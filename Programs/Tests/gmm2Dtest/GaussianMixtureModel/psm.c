//
//  psm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/12/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "psm.h"

void InitializePSM( psm_t * model )
{
    GMMFunctions.Model.Initialize( &model->gmm );
//    KumaraswamyFunctions.Initialize( &model->kumaraswamy, NUM_STATES );
    model->state_bands.length = NUM_STATE_GROUPS;
    memset( model->state_bands.band, 0, sizeof(model->state_bands.band) );
}

void ReportObservationsPSM( psm_t * model, observation_t * observations, uint8_t num_observations )
{
    /* Cycle through and add observations to gaussian mixture model */
    for( uint8_t i = 0; i < num_observations && i < MAX_OBSERVATIONS; i++ )
    {
        vec2 value = { observations[i].a, observations[i].b };
        GMMFunctions.Model.AddValue( &model->gmm, &observations[i], &value );
    }
//    model->hmm.M = model->gmm.num_clusters;
    // Analyse value
}

void UpdatePSM( psm_t * model, double nu )
{
    /* Update state bands */
    double state_bands[NUM_STATE_GROUPS] = { 0. };
    PSMFunctions.UpdateStateBands( model, nu, state_bands, NUM_STATE_GROUPS );
    
    /* Update states/transition matrix */
//    FSMFunctions.Sys.Update( &model->hmm.A, state_bands );
    
    /* Calculate best cluster/observation and update observation matrix */
    model->observation_state = PSMFunctions.GetCurrentBand( model, &model->state_bands );
//    model->hmm.T = addToObservationBuffer( &model->hmm.O, model->observation_state );

    /* Update state path prediction to best cluster */
//    HMMFunctions.UpdateObservationMatrix( &model->hmm );
    
//    model->best_state = PSMFunctions.FindMostLikelyHiddenState( model, TARGET_STATE, &model->best_confidence );
//    PSMFunctions.UpdateBestCluster( model, &model->state_bands );
    
    /* Generate proposals to complete update */
//    PSMFunctions.GenerateProposals( model );
}

void UpdateStateBandsPSM( psm_t * model, double nu, double * bands, uint8_t num_bands )
{
//    PSMFunctions.InfluenceStateBands( model, &model->state_bands );
    
    PSMFunctions.DiscoverStateBands( model, &model->state_bands );
//    KumaraswamyFunctions.GetVector( &model->kumaraswamy, nu, bands, &model->state_bands );
}

//void InfluenceStateBandsPSM( psm_t * model, band_list_t * band_list )
//{
////    cluster_boundary_list_t cluster_boundaries = { MAX_CLUSTERS*2, 0 };
////    if( model->gmm.num_clusters )
////        GMMFunctions.Model.SortClusterBoundaries( &model->gmm, &cluster_boundaries );
////    else
////    {}
////    PSMFunctions.FindStateBandCenters( model, &cluster_boundaries, band_list );
//}

static void UpdateBand( band_list_t * band_list, uint8_t i, int8_t c, gaussian2d_t * band_gaussian )
{
    if( i > band_list->length ) return;
    if( c == 0 )
    { /* If no gaussian for band, zero state info */
        if( !i )
        {
            band_list->band[i].lower_boundary = 700.;
            band_list->band[i].upper_boundary = 700.;
            band_list->band[i].true_center = (vec2){ 0., 0. };
        }
        else
        {
            band_list->band[i].lower_boundary = band_list->band[i-1].lower_boundary;
            band_list->band[i].upper_boundary = band_list->band[i-1].upper_boundary;
            band_list->band[i].true_center = band_list->band[i-1].true_center;
        }
    }
    else if( c == -1 )
    {
        double boundary = band_list->band[i-1].upper_boundary;
        band_list->band[i].lower_boundary = boundary;
        band_list->band[i].upper_boundary = boundary;
        band_list->band[i].true_center = (vec2){ band_list->band[i-1].true_center.a, boundary };
    }
    else
    { /* Otherwise set using cumulated band gaussian */
        double radius = band_gaussian->covariance.d * VALID_CLUSTER_STD_DEV;
        band_list->band[i].lower_boundary = band_gaussian->mean.b + radius;
        band_list->band[i].upper_boundary = band_gaussian->mean.b - radius;
        band_list->band[i].true_center = band_gaussian->mean;
        if(i)
        {
            band_list->band[i-1].upper_boundary = band_list->band[i].lower_boundary;
        }
    }
}

void DiscoverStateBandsPSM( psm_t * model, band_list_t * band_list )
{
    uint32_t processed_clusters = { 0 };
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
        GMMFunctions.Cluster.UpdateLimits( model->gmm.cluster[i] );
    
    gaussian2d_t band_gaussian = { 0 };
    band_list->band[0].lower_boundary = 700;
    
    int8_t current_band_id = 0, num_clusters_in_band = 0, num_to_process = model->gmm.num_clusters;
    uint32_t running_label_vector = 0;
    while(num_to_process-- > 0)
    {
        /* Get next min cluster */
        uint8_t min_id = 0;
        double min_boundary = 0, check_boundary;
        for(uint32_t i = 0, m = 1; i < model->gmm.num_clusters; i++, m <<= 1 )
        {
            if( processed_clusters & m ) continue;
            check_boundary = model->gmm.cluster[i]->max_y;
            if( check_boundary > min_boundary )
            {
                min_boundary = check_boundary;
                min_id = i;
            }
        }
        
        /* Check if new cluster has new label(s) */
        uint32_t current_label_vector = GetValidLabels( &model->gmm.cluster[min_id]->labels );
        uint32_t check = current_label_vector & ~running_label_vector;
        if( check )
        { /* If new labels in cluster */
            uint32_t new_label_vector = running_label_vector | current_label_vector;
            /* If new update skipped bands, if any */
            current_band_id = CountSet(new_label_vector);
            for( uint8_t i = CountSet(running_label_vector); i < current_band_id; i++ )
            {
                UpdateBand( band_list, i, num_clusters_in_band, &band_gaussian );
                num_clusters_in_band = -1;
            }
            band_gaussian = model->gmm.cluster[min_id]->gaussian_in;
            num_clusters_in_band = 1;
//            UpdateBand( band_list, current_band_id-1, num_clusters_in_band, &band_gaussian );
            running_label_vector = new_label_vector;
        }
        else
        { /* Otherwise cumulate current gaussian into band gaussian */
            mulGaussian2d( &band_gaussian, &model->gmm.cluster[min_id]->gaussian_in, &band_gaussian );
            num_clusters_in_band++;
        }
        
        if( !num_to_process )
        {
            UpdateBand( band_list, current_band_id, num_clusters_in_band, &band_gaussian );
        }
        processed_clusters |= 1 << min_id;
    }
    for( uint8_t i = current_band_id+1; i < band_list->length; i++ )
        UpdateBand( band_list, i, -1, NULL );
    band_list->band[band_list->length-1].upper_boundary = 0;

    printf("State bands: \n");
    for( uint8_t i = 0; i < band_list->length; i++ )
        printf(" %d: (%.3f %.3f)  C<%.3f %.3f>\n", i, band_list->band[i].lower_boundary, band_list->band[i].upper_boundary, band_list->band[i].true_center.a, band_list->band[i].true_center.b);
    printf("\n");
    
//
//
//    for( uint8_t i = 0; i < cluster_boundaries->length ; i++ )
//    {
//        /* Estimate mean of cluster in band */
//        uint8_t label = cluster_boundaries->list[i].label;
//        gaussian_mixture_cluster_t * cluster = model->gmm.cluster[label];
//        double mean_y_offset = band_list->band[band_id].true_center.b - cluster->gaussian_out.mean.b;
//        double mean_x_offset = GetMeanXAtGaussianYOffset( &cluster->gaussian_out, mean_y_offset );
//        double max_contrib = cluster->labels.average[0];
//        for( uint j = 1; j < MAX_LABELS; j++ )
//        {
//            if( max_contrib < cluster->labels.average[j] )
//                max_contrib = cluster->labels.average[j];
//        }
//
//        /* Factored mean accounts for contributing blobs in cluster */
//        double factored_mean_x_offset = mean_x_offset * ZDIV( 1., max_contrib );
//        if( BOUNDARY_START(label) )
//        {
//            num_in_band++;
//            band_nu += factored_mean_x_offset;
//        }
//        else
//        {
//            num_in_band--;
//            band_nu -= factored_mean_x_offset;
//        }
//
//        /* Ignore invalid band (bands with too many labels aka too much noise) */
//        valid_band = num_in_band < band_list->length;
//
//        /* Update band id is less than new band, start new observation group */
//        if( valid_band &&
//           ( band_id < num_in_band ) )
//        {
//            band_id = num_in_band;
//
//            if( band_id > 0 )
//            {
//                band_list->band[band_id].lower_boundary = cluster_boundaries->list[i].value;
//                band_list->band[band_id].true_center.a = band_nu;
//            }
//
//            num_observation_groups++;
//        }
//    }
//    model->hmm.B.num_observation_symbols = num_observation_groups;
}
//
//void FindTrueCentersOfStateBandsPSM( psm_t * model, cluster_boundary_list_t * cluster_boundaries, band_list_t * band_list )
//{
//    /* Cycle down clusters and count ones that open (have max) in band */
//    gaussian1d_t band_gaussian = { 0, 0 };
//    uint8_t band_elements[MAX_CLUSTERS] = { 0 }, num_band_elements = 0;
//    bool bump_to_next_band = false;
//    for( uint8_t i = band_list->length - 1; i >= 0; i-- )
//    {
//        if( !bump_to_next_band )
//        {
//            band_gaussian.mean = 0;
//            band_gaussian.std_dev = 0;
//        }
//        else
//            bump_to_next_band = false;
//
//        for( uint8_t j = cluster_boundaries->length - 1; j >= 0; j-- )
//        {
//            uint8_t label = cluster_boundaries->list[j].label;
//            double lower_boundary = band_list->band[i].lower_boundary;
//            if( BOUNDARY_START(label) )
//            {
//                if( cluster_boundaries->list[j].value >= lower_boundary)
//                {
//                    gaussian1d_t new_gaussian = getGaussian1dFrom2dY( &model->gmm.cluster[label]->gaussian_out );
//                    /* Copy if band gaussian is not initialized */
//                    if( !band_gaussian.std_dev )
//                        copyGaussian1d( &new_gaussian, &band_gaussian );
//                    /* Combine if not */
//                    else
//                        mulGaussian1d( &band_gaussian, &new_gaussian, &band_gaussian );
//                    band_elements[num_band_elements++] = label;
//                }
//            }
//            else if( --num_band_elements > 0 )
//            {/* If next band is not empty, recalculated band_gaussian */
//                gaussian1d_t new_gaussian = getGaussian1dFrom2dY( &model->gmm.cluster[band_elements[0]]->gaussian_out );
//                copyGaussian1d( &new_gaussian, &band_gaussian );
//
//                /* Contruct band gaussian without ended gaussian */
//                for( uint8_t ii = 1; ii < num_band_elements; ii++ )
//                {
//                    new_gaussian = getGaussian1dFrom2dY( &model->gmm.cluster[band_elements[ii]]->gaussian_out );
//                    mulGaussian1d( &band_gaussian, &new_gaussian, &band_gaussian );
//                }
//            }
//            else
//            { /* Artificially calculated center of empty bands */
//                double upper_boundary = (i+2 >= band_list->length)
//                ? MAX_THRESH
//                : band_list->band[i+1].lower_boundary,
//                band_mean = (upper_boundary - lower_boundary) / 2,
//                band_std_dev = ( band_mean - lower_boundary ) / 2;
//                band_gaussian = (gaussian1d_t){ band_mean, band_std_dev };
//            }
//        }
//        band_list->band[i].true_center.b = band_gaussian.mean;
//    }
//}

uint8_t FindMostLikelyHiddenStatePSM( psm_t * model, uint8_t observation_state, double * confidence )
{
    uint8_t best_observation_id = 0;
//    double best_observation_weight = 0.;
//    
//    /* Determine target observation band */
//    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
//    {
//        double check = model->hmm.B.expected[TARGET_STATE][i];
//        if( check > best_observation_weight )
//        {
//            best_observation_id = i;
//            best_observation_weight = check;
//        }
//    }
//    if( confidence != NULL) *confidence = best_observation_weight;
    return best_observation_id;
}

void UpdateBestClusterPSM( psm_t * model, band_list_t * band_list )
{
    double
        lower_bound = band_list->band[model->best_state].lower_boundary,
        upper_bound = ( model->best_state + 1 >= NUM_STATE_GROUPS ) ?
            MAX_THRESH : band_list->band[model->best_state + 1].lower_boundary,
        best_cluster_weight = 0.;
    int8_t best_cluster_id = -1;
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
    {
        gaussian_mixture_cluster_t * cluster = model->gmm.cluster[i];
        if( IN_RANGE( cluster->gaussian_out.mean.b, lower_bound, upper_bound ) )
        {
            GMMFunctions.Cluster.Weigh( cluster );
            if( cluster->weight > best_cluster_weight )
            {
                best_cluster_weight = cluster->weight;
                best_cluster_id = i;
            }
        }
    }
    model->best_cluster_id = best_cluster_id;
    model->best_cluster_weight = best_cluster_weight;
}

uint8_t GetCurrentBandPSM( psm_t * model, band_list_t * band_list )
{
    uint8_t state = 0;
    for( ; state < band_list->length-1; state++ )
    {
        /* If next boundary is abovet thresh, band is in current thresh */
        if( model->previous_thresh < band_list->band[state+1].lower_boundary )
            break;
    }
    return state;
}

void GenerateProposalsPSM( psm_t * model )
{
    /* Update predictions */
    vec2 * proposed_center = &model->state_bands.band[ model->best_state ].true_center;
    model->proposed_nu = proposed_center->a;
    model->proposed_thresh = proposed_center->b;
    
    /* Update primary & secondary to be reconstructed */
    gaussian_mixture_cluster_t * cluster = model->gmm.cluster[model->best_cluster_id];
    model->proposed_primary_id = cluster->primary_id;
    model->proposed_primary_id = cluster->secondary_id;
    
    /* Update current state */
//    model->current_state = PSMFunctions.FindMostLikelyHiddenState( model, model->hmm.A.state, NULL );
}
