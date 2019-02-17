//
//  gmm.c
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "gmm.h"

void InitializeGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output, double variance )
{
    cluster->gaussian_in.mean.a = observation->a;
    cluster->gaussian_in.mean.b = observation->b;
    cluster->gaussian_out.mean.a = output->a;
    cluster->gaussian_out.mean.b = output->b;
    
    memset( &cluster->gaussian_out.covariance, 0, sizeof(mat2x2) );
    cluster->gaussian_in.covariance.a = variance; cluster->gaussian_in.covariance.d = variance;
    cluster->gaussian_in.covariance.b = 0.; cluster->gaussian_in.covariance.c = 0.;
    cluster->score = 1.;
    
    memset( &cluster->labels, 0, sizeof(cluster->labels) );
    cluster->labels.average[observation->label] = 1;
    cluster->labels.count[observation->label]++;
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
}
void UpdateGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output )
{
    if (cluster->mahalanobis_sq > MAX_MAHALANOBIS_SQ_FOR_UPDATE)
        return;
    double weight = ALPHA * safe_exp( -BETA * cluster->mahalanobis_sq );
    cluster->score += weight * ( cluster->probability_condition_input - cluster->score );
    weight = ALPHA * cluster->probability_condition_input;
    
    UpdateCovarianceWithWeight( (vec2 *)observation, &cluster->gaussian_in,  weight );
    UpdateCovarianceWithWeight( output, &cluster->gaussian_out, weight );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
    GMMFunctions.Cluster.UpdateLimits( cluster );
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    getMat2x2Inverse( &cluster->gaussian_in.covariance, &cluster->inv_covariance_in );
    
    ReportLabel( &cluster->labels, observation->label );
}
void GetScoreOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input)
{
    vec2 input_delta;
    vec2SubVec2(&cluster->gaussian_in.mean, input, &input_delta);
    cluster->mahalanobis_sq = CalculateMahalanobisDistanceSquared( &cluster->inv_covariance_in, &input_delta);
    double half_mahalanobis_sq = 0.5 * cluster->mahalanobis_sq;
    cluster->score = safe_exp( cluster->log_gaussian_norm_factor - half_mahalanobis_sq );
}
void UpdateNormalOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    double cholesky_dms = cluster->llt_in.a * cluster->llt_in.d, norm_factor = -(double)(ZDIV_LNUM);
    if( cholesky_dms ) norm_factor = -log( M_2_PI * sqrt( cholesky_dms ) );
    cluster->log_gaussian_norm_factor = norm_factor;
}
void UpdateInputProbabilityOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, double total_probability )
{
    cluster->probability_condition_input = ZDIV( cluster->score, total_probability );
}
void ContributeToOutputOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input, vec2 * output )
{
    vec2 input_delta;
    vec2SubVec2(&cluster->gaussian_in.mean, input, &input_delta);
    vec2 inv_covariance_delta;
    mat2x2DotVec2(&cluster->inv_covariance_in, &input_delta, &inv_covariance_delta);
    
    vec2 input_covariance, pre_condition, pre_output;
    mat2x2DotVec2( &cluster->gaussian_out.covariance, &inv_covariance_delta, &input_covariance );
    vec2AddVec2( &cluster->gaussian_out.mean, &input_covariance, &pre_condition);
    scalarMulVec2( cluster->probability_condition_input, &pre_condition, &pre_output );
    vec2AddVec2( output, &pre_output, output);
}
void UpdateLimitsOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    double radius_y = cluster->gaussian_out.covariance.d * VALID_CLUSTER_STD_DEV;
    cluster->max_y = cluster->gaussian_out.mean.b + radius_y;
    cluster->min_y = cluster->gaussian_out.mean.b - radius_y;
}
void WeighGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    /* Find best two label contributes */
    double first = cluster->labels.average[0], second = 0.;
    for( uint8_t i = 1; i < MAX_LABELS; i++ )
    {
        double check = cluster->labels.average[i];
        if( check > first )
        {
            second = first;
            first = check;
        }
        else if ( check > second )
            second = check;
    }
    double a = ( cluster->gaussian_out.covariance.b * cluster->gaussian_out.covariance.c ),
    b = ( cluster->gaussian_out.covariance.a * cluster->gaussian_out.covariance.d );
    double eccentricity_factor = ZDIV( a, b );
    cluster->weight = ( first + second ) * eccentricity_factor;
    cluster->primary_id = first;
    cluster->secondary_id = second;
}

void InitializeGaussianMixtureModel( gaussian_mixture_model_t * model )
{
    memset( model, 0, sizeof(gaussian_mixture_model_t) );
}
double GetScoreSumOfClustersInGaussianMixtureModel( gaussian_mixture_model_t * model, vec2 * input )
{
    double score_sum = 0;
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = &model->cluster[i];
        GMMFunctions.Cluster.GetScore( cluster, input );
        score_sum += cluster->score;
    }
    return score_sum;
}
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t * model, double total_probability, vec2 * input, vec2 * output )
{
    double best_match_distance = MAX_DISTANCE;
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = &model->cluster[i];
        GMMFunctions.Cluster.UpdateInputProbability( cluster, total_probability );
        GMMFunctions.Cluster.ContributeToOutput( cluster, input, output );
        if( cluster->mahalanobis_sq > best_match_distance )
            best_match_distance = cluster->mahalanobis_sq;
    }
    return best_match_distance;
}
double GetMaxErrorOfGaussianMixtureModel( gaussian_mixture_model_t * model, vec2 * output, vec2 * value, vec2 * min_max_delta )
{
    vec2 output_delta;
    vec2SubVec2( value, output, &output_delta );
    double a_error = fabs( ZDIV( output_delta.a, min_max_delta->a ) ),
    b_error = fabs( ZDIV( output_delta.b, min_max_delta->b ) );
    return MAX( a_error, b_error );
}
void AddClusterToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    uint8_t new_index = model->num_clusters;
    GMMFunctions.Cluster.Initialize( &model->cluster[new_index], observation, value, model->initial_variance );
    model->num_clusters++;
}
void UpdateGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++ )
    {
        cluster = &model->cluster[i];
        GMMFunctions.Cluster.Update( cluster, observation, value );
    }
}
void AddValueToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    vec2 output = { 0., 0. };
    double total_probability = GMMFunctions.Model.GetScoreSumOfClusters( model, (vec2 *)observation );
    double best_distance = GMMFunctions.Model.GetOutputAndBestDistance( model, total_probability, (vec2 *)observation, &output);
    
    vec2 min_max_delta;
    vec2SubVec2( &model->max_out, &model->min_out, &min_max_delta );
    double max_error = GMMFunctions.Model.GetMaxError( model, &output, value, &min_max_delta );
    
    GMMFunctions.Model.Update( model, observation, value );
    
    /* Add cluster if error or distance is to high for a cluster match */
    if( !model->num_clusters
       || ( ( max_error > MAX_ERROR )
           && ( best_distance > MIN_MAHALANOBIS_DISTANCE_SQ ) ) )
        GMMFunctions.Model.AddCluster( model, observation, value );
}
void SortClusterBoundariesOfGaussianMixtureModel( gaussian_mixture_model_t * model, cluster_boundary_list_t * cluster_boundaries )
{
    /* Cluster sort list
     * - Min boundaries are sorted as -<index+1>
     * - Max boundaries are sorted as +<index+1>
     * - Index is offset one to account for index 0
     */
    
    /* Initialize list ascending list of all clusters to sort */
    uint8_t sorted[MAX_CLUSTERS], left_to_sort = model->num_clusters, num_boundaries = 0;
    for( uint8_t i = 0; i < model->num_clusters; i++ )
        sorted[i] = i+1;
    
    /* Cycle and sort all cluster boundaries */
    do
    {
        for( uint8_t i = 0; i < model->num_clusters; i++ )
        {
            /* Skip clusters already sorted */
            if( sorted[i] == 0 ) continue;
            
            /* Reset status variables */
            bool next_is_min = true;
            double next_boundary = model->cluster[0].min_y;
            int8_t next_cluster = 1;
            
            /* Cycle through clusters to find next boundary (could be min or max of a cluster) */
            for( uint8_t j = 1; j < model->num_clusters; j++ )
            {
                if( model->cluster[j].min_y < next_boundary )
                {
                    next_boundary = model->cluster[j].min_y;
                    next_cluster = j+1;
                    next_is_min = true;
                }
                if( model->cluster[j].max_y < next_boundary )
                {
                    next_boundary = model->cluster[j].max_y;
                    next_cluster = j+1;
                    next_is_min = false;
                }
            }
            
            /* Store boundary in cluster boundaries and mark cluster as sorted if ended */
            cluster_boundary_t next_cluster_boundary = (cluster_boundary_t){ next_boundary, -next_cluster };
            if( !next_is_min )
            {
                next_cluster_boundary.label *= -1;
                sorted[next_cluster-1] = 0;
                left_to_sort--;
            }
            cluster_boundaries->list[num_boundaries++] = next_cluster_boundary;
            
            /* End if sort list is filled */
            if( num_boundaries >= cluster_boundaries->length )
            {
                left_to_sort = 0;
                break;
            }
        }
    } while( left_to_sort > 0 );
}
