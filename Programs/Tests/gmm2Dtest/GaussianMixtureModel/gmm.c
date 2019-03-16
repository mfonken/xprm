//
//  gmm.c
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "gmm.h"

void InitializeGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output )
{
    cluster->gaussian_in.mean.a = observation->a;
    cluster->gaussian_in.mean.b = observation->b;
    cluster->gaussian_out.mean.a = output->a;
    cluster->gaussian_out.mean.b = output->b;
    
    memset( &cluster->gaussian_out.covariance, 0, sizeof(mat2x2) );
    cluster->gaussian_in.covariance = (mat2x2){ INITIAL_VARIANCE, 0, 0, INITIAL_VARIANCE };
    cluster->inv_covariance_in = (mat2x2){ INV_INITIAL_VARIANCE, 0, 0, INV_INITIAL_VARIANCE };
    cluster->score = 1.;
    
    memset( &cluster->labels, 0, sizeof(cluster->labels) );
    cluster->labels.average[observation->label] = 1;
    cluster->labels.count[observation->label]++;
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
}
void UpdateGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, observation_t * observation, vec2 * output )
{
//    printf("m_maha: %.2f\n", cluster->mahalanobis_sq);
    if (cluster->mahalanobis_sq > MAX_MAHALANOBIS_SQ_FOR_UPDATE)
    {
//        printf("m_invm: %.2f\n", cluster->mahalanobis_sq);
        return;
    }
    double score_weight = ALPHA * safe_exp( -BETA * cluster->mahalanobis_sq );
    cluster->score += score_weight * ( cluster->probability_condition_input - cluster->score );
    
    double weight = ALPHA * cluster->probability_condition_input;
    
    vec2 delta_mean_in = WeightedIncreaseMean( (vec2 *)observation, &cluster->gaussian_in, weight );
    vec2 delta_mean_out = WeightedIncreaseMean( output, &cluster->gaussian_out, weight );
    
    //printf("m_mean: [%.2f %.2f] | m_dmin: <%.2f %.2f> | m_dmou: <%.2f %.2f>\n",
//           cluster->gaussian_in.mean.a, cluster->gaussian_in.mean.b,
//           delta_mean_in.a, delta_mean_in.b, delta_mean_out.a, delta_mean_out.b);
    
    UpdateCovarianceWithWeight( &delta_mean_in, &delta_mean_in,  &cluster->gaussian_in,  weight );
    UpdateCovarianceWithWeight( &delta_mean_in, &delta_mean_out, &cluster->gaussian_out, weight );
    
    GetMat2x2LLT( &cluster->gaussian_in.covariance, &cluster->llt_in );
    getMat2x2Inverse( &cluster->gaussian_in.covariance, &cluster->inv_covariance_in );
    
    GMMFunctions.Cluster.UpdateNormal( cluster );
    
    ReportLabel( &cluster->labels, observation->label );
}
void GetScoreOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input)
{
    vec2 input_delta;
    vec2SubVec2(input, &cluster->gaussian_in.mean, &input_delta);
    cluster->mahalanobis_sq = CalculateMahalanobisDistanceSquared( &cluster->inv_covariance_in, &input_delta);
    cluster->probability_of_in = safe_exp( cluster->log_gaussian_norm_factor - 0.5 * cluster->mahalanobis_sq );
    //printf("m_prin: %f | m_masq: %.2f | m_lgnf: %.2f | m_icov: [ %.2f %.2f | %.2f %.2f ] | m_indt: < %.2f %.2f>\n",
//           cluster->probability_of_in, cluster->mahalanobis_sq, cluster->log_gaussian_norm_factor,
//           cluster->inv_covariance_in.a, cluster->inv_covariance_in.b, cluster->inv_covariance_in.c, cluster->inv_covariance_in.d,
//           input_delta.a, input_delta.b );
}
void UpdateNormalOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    //printf("m_norm: [%.2f %.2f | %.2f %.2f]", cluster->llt_in.a, cluster->llt_in.b, cluster->llt_in.c, cluster->llt_in.d);
    double cholesky_dms = cluster->llt_in.a * cluster->llt_in.d,
    norm_factor = -log( 2 * M_PI * sqrt( cluster->llt_in.a ) * sqrt( cluster->llt_in.d ) );
    //printf(" %.2f %.2f\n", cholesky_dms, norm_factor);
    cluster->log_gaussian_norm_factor = norm_factor;
}
void UpdateInputProbabilityOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, double total_probability )
{
    cluster->probability_condition_input = total_probability < MIN_TOTAL_MIXTURE_PROBABILITY
    ? 0.f: ZDIV( cluster->probability_of_in, total_probability );
    
    //printf("m_sipr: %.2f | m_prci: %.2f\n", total_probability, cluster->probability_condition_input);
}
void ContributeToOutputOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster, vec2 * input, vec2 * output )
{
    vec2 input_delta;
    vec2SubVec2(input, &cluster->gaussian_in.mean, &input_delta);
    vec2 inv_covariance_delta;
    mat2x2DotVec2(&cluster->inv_covariance_in, &input_delta, &inv_covariance_delta);
    
    vec2 input_covariance, pre_condition, pre_output;
    mat2x2 cov_out_T = { cluster->gaussian_out.covariance.a, cluster->gaussian_out.covariance.c,
                         cluster->gaussian_out.covariance.b, cluster->gaussian_out.covariance.d };
    mat2x2DotVec2( &cov_out_T, &inv_covariance_delta, &input_covariance );
    vec2AddVec2( &cluster->gaussian_out.mean, &input_covariance, &pre_condition);
    scalarMulVec2( cluster->probability_condition_input, &pre_condition, &pre_output );
    vec2AddVec2( output, &pre_output, output);
}
void UpdateLimitsOfGaussianMixtureCluster( gaussian_mixture_cluster_t * cluster )
{
    double radius_y = cluster->gaussian_in.covariance.d * VALID_CLUSTER_STD_DEV;
    cluster->max_y = cluster->gaussian_in.mean.b + radius_y;
    cluster->min_y = cluster->gaussian_in.mean.b - radius_y;
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
    for( uint16_t i = 0; i < MAX_CLUSTERS; i++ )
        model->cluster[i] = &(model->cluster_mem[i]);
}
double GetScoreSumOfClustersInGaussianMixtureModel( gaussian_mixture_model_t * model, vec2 * input )
{
    double score_sum = 0;
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.GetScore( cluster, input );
        score_sum += cluster->probability_of_in;
    }
    return score_sum;
}
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t * model, double total_probability, vec2 * input, vec2 * output )
{
    double best_match_distance = MAX_DISTANCE;
    gaussian_mixture_cluster_t * cluster;
    //printf("m_numc: %d | m_totp: %f\n", model->num_clusters, total_probability);
    for( uint8_t i = 0; i < model->num_clusters; i++)
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.UpdateInputProbability( cluster, total_probability );
        //printf("m_cscr: %f\n", cluster->probability_condition_input);
        if( cluster->probability_condition_input > MIN_CLUSTER_SCORE)
        {
            GMMFunctions.Cluster.ContributeToOutput( cluster, input, output );
        }
        if( cluster->mahalanobis_sq < best_match_distance )
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
    //printf("m_mmdt: <%.2f %.2f> | m_vodt: <%.2f %.2f> | a:%.2f b:%.2f\n",min_max_delta->a, min_max_delta->b, output_delta.a, output_delta.b, a_error, b_error);
    return a_error > b_error ? a_error : b_error;
}
void AddClusterToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    uint16_t i = 0;
    for( ; i < model->num_clusters; i++ )
        if( model->cluster[i] == NULL ) break;
    GMMFunctions.Cluster.Initialize( model->cluster[i], observation, value );
    model->num_clusters++;
}
void UpdateGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    gaussian_mixture_cluster_t * cluster;
    for( uint8_t i = 0; i < model->num_clusters; i++ )
    {
        cluster = model->cluster[i];
        GMMFunctions.Cluster.Update( cluster, observation, value );
    }
    for( uint8_t i = 0; i < model->num_clusters; i++ )
    {
        if( (*model->cluster)[i].score < MIN_CLUSTER_SCORE)
            GMMFunctions.Model.RemoveCluster( model, i );
    }
}
void AddValueToGaussianMixtureModel( gaussian_mixture_model_t * model, observation_t * observation, vec2 * value )
{
    if( !model->num_clusters )
    {
        model->min_in = *(vec2 *)( observation );
        model->max_in = model->min_in;
        model->min_out = *value;
        model->max_out = model->min_out;
    }
    else
    {
        model->min_in  = (vec2){ MIN( model->min_in.a, observation->a ), MIN( model->min_in.b, observation->b ) };
        model->max_in  = (vec2){ MAX( model->max_in.a, observation->a ), MAX( model->max_in.b, observation->b ) };
        model->min_out = (vec2){ MIN( model->min_out.a, value->a ),      MIN( model->min_out.b, value->b )      };
        model->max_out = (vec2){ MAX( model->max_out.a, value->a ),      MAX( model->max_out.b, value->b )      };
    }
    //printf("m_mini: <%.2f %.2f> | m_maxi: <%.2f %.2f> | m_mino: <%.2f %.2f> | m_maxo: <%.2f %.2f>\n",
//           model->min_in.a, model->min_in.b, model->max_in.a, model->max_in.b,
//           model->min_out.a, model->min_out.b, model->max_out.a, model->max_out.b);
    vec2 output = { 0., 0. };
    double total_probability = GMMFunctions.Model.GetScoreSumOfClusters( model, (vec2 *)observation );
    double best_distance = GMMFunctions.Model.GetOutputAndBestDistance( model, total_probability, (vec2 *)observation, &output);
    
    //printf("m_outp: <%.2f %.2f>\n", output.a, output.b);
    
    vec2 min_max_delta;
    vec2SubVec2( &model->max_out, &model->min_out, &min_max_delta );
//    printf("m_mmdl: <%.3f %.3f> | <%.3f %.3f> | <%.3f %.3f>\n", model->max_out.a, model->max_out.b, model->min_out.a, model->min_out.b, min_max_delta.a, min_max_delta.b);
    double max_error = GMMFunctions.Model.GetMaxError( model, &output, value, &min_max_delta );
//    printf("m_maxe: %.2f | m_best: %.2f\n", max_error, best_distance);
//    printf("Max error: %.2f\n", max_error);
    /* Add cluster if error or distance is to high for a cluster match */
    if( model->num_clusters < MAX_CLUSTERS )
    {
        if( !model->num_clusters
           || ( ( max_error > MAX_ERROR ) && ( best_distance > MAX_MAHALANOBIS_SQ ) ) )
            GMMFunctions.Model.AddCluster( model, observation, value );
    }
    
    GMMFunctions.Model.Update( model, observation, value );
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
    {
        GMMFunctions.Cluster.UpdateLimits( model->cluster[i] );
        sorted[i] = i+1;
    }
    
    /* Cycle and sort all cluster boundaries */
    do
    {
        for( uint8_t i = 0; i < model->num_clusters; i++ )
        {
            /* Skip clusters already sorted */
            if( sorted[i] == 0 ) continue;
            
            /* Reset status variables */
            bool next_is_min = true;
            double next_boundary = (*model->cluster)[i].min_y;
            int8_t next_cluster = i + 1;
            
            /* Cycle through clusters to find next boundary (could be min or max of a cluster) */
            for( uint8_t j = i + 1; j < model->num_clusters; j++ )
            {
                if( (*model->cluster)[j].max_y < next_boundary )
                {
                    next_boundary = (*model->cluster)[j].max_y;
                    next_cluster = j+1;
                    next_is_min = false;
                }
                if( (*model->cluster)[j].min_y < next_boundary )
                {
                    next_boundary = (*model->cluster)[j].min_y;
                    next_cluster = j+1;
                    next_is_min = true;
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
void RemovClusterFromGaussianMixtureModel( gaussian_mixture_model_t * model, uint16_t index )
{
    model->cluster[index] = NULL;
    model->num_clusters--;
}
