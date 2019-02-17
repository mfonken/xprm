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
    KumaraswamyFunctions.Initialize( &model->kumaraswamy, NUM_STATES );
    model->state_bands.length = NUM_STATE_GROUPS;
    memset( model->state_bands.band, 0, sizeof(model->state_bands.band) );
}

void ReportObservationsPSM( psm_t * model, observation_t * observations, uint8_t num_observations )
{
    /* Cycle through and add observations to gaussian mixture model */
    vec2 value;
    for( uint8_t i = 0; i < num_observations && i < MAX_OBSERVATIONS; i++ )
        GMMFunctions.Model.Update( &model->gmm, &observations[i], &value );
    model->hmm.M = model->gmm.num_clusters;
    // Analyse value
}

void UpdatePSM( psm_t * model, double nu )
{
    /* Update state intervals */
    double state_intervals[NUM_STATE_GROUPS] = { 0. };
    PSMFunctions.UpdateStateIntervals( model, nu, state_intervals, NUM_STATE_GROUPS );
    
    /* Update states/transition matrix */
    FSMFunctions.Sys.Update( &model->hmm.A, state_intervals );
    
    /* Update observation matrix */
    uint8_t estimated_state = PSMFunctions.GetCurrentBand( model, &model->state_bands );
    for( uint8_t i = 0; i < MAX_OBSERVATIONS; i++ )
    {
        model->hmm.B.map[i][estimated_state] += model->gmm.cluster[i].weight;
        model->hmm.B.map[i][estimated_state] /= 2.;
    }
    
    /* Calculate best cluster/observation */
    uint8_t best_cluster_id = PSMFunctions.FindBestCluster( model );
    addToObservationBuffer( &model->hmm.O, best_cluster_id );
    model->hmm.T = model->hmm.O.next-1;
    
    /* Update state path prediction to best cluster */
    HMMFunctions.ForwardSolve( &model->hmm );
    
    /* Update predictions */
    vec2 * proposed_center = &model->state_bands.band[ model->hmm.best_state ].true_center;
    model->proposed_nu = proposed_center->a;
    model->proposed_thresh = proposed_center->b;
    
    /* Update primary & secondary to reconstruct */
    gaussian_mixture_cluster_t * cluster = &model->gmm.cluster[best_cluster_id];
    model->proposed_primary_id = cluster->primary_id;
    model->proposed_primary_id = cluster->secondary_id;
}

void UpdateStateIntervalsPSM( psm_t * model, double nu, double * intervals, uint8_t num_intervals )
{
    PSMFunctions.InfluenceStateBands( model, &model->state_bands );
    KumaraswamyFunctions.GetVector( &model->kumaraswamy, nu, intervals, &model->state_bands );
}

void InfluenceStateBandsPSM( psm_t * model, band_list_t * band_list )
{
    cluster_boundary_list_t cluster_boundaries = { MAX_CLUSTERS*2, 0 };
    GMMFunctions.Model.SortClusterBoundaries( &model->gmm, &cluster_boundaries );
    PSMFunctions.FindStateBandLowerBoundaries( model, &cluster_boundaries, band_list );
    PSMFunctions.FindStateBandCenters( model, &cluster_boundaries, band_list );
}

void FindLowerBoundariesOfStateBandPSM( psm_t * model, cluster_boundary_list_t * cluster_boundaries, band_list_t * band_list )
{
    uint8_t cluster_label_counts[MAX_CLUSTERS] = { 0 };
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
        cluster_label_counts[i] = GetNumLabels( &model->gmm.cluster[i].labels );
    
    uint8_t band_id = 0, num_in_band = 0, num_observation_groups = 0;
    bool valid_band = false;
    double band_nu = 0;
    for( uint8_t i = 0; i < cluster_boundaries->length ; i++ )
    {
        uint8_t label = cluster_boundaries->list[i].label;
        if( label < 0 )
        {
            num_in_band++;
            band_nu += model->gmm.cluster[label].gaussian_out.mean.a;
        }
        else if( label > 0 )
        {
            num_in_band--;
            band_nu -= model->gmm.cluster[label].gaussian_out.mean.a;
        }
        
        /* Ignore invalid band (bands with too many labels aka too much noise) */
        valid_band = num_in_band < band_list->length;
        
        /* Update band id is less than new band, start new observation group */
        if( valid_band &&
           ( band_id < num_in_band ) )
        {
            band_id = num_in_band;
            
            if( band_id > 0 )
            {
                band_list->band[band_id].lower_boundary = cluster_boundaries->list[i].value;
                band_list->band[band_id].true_center.a = band_nu;
            }
            
            num_observation_groups++;
        }
    }
    model->hmm.B.num_observation_symbols = num_observation_groups;
}

void FindTrueCentersOfStateBandsPSM( psm_t * model, cluster_boundary_list_t * cluster_boundaries, band_list_t * band_list )
{
    /* Cycle down clusters and count ones that open (have max) in band */
    gaussian1d_t band_gaussian = { 0, 0 };
    bool bump_to_next_band = false;
    for( uint8_t i = band_list->length - 1; i >= 0; i-- )
    {
        if( !bump_to_next_band )
        {
            band_gaussian.mean = 0;
            band_gaussian.std_dev = 0;
        }
        else
            bump_to_next_band = false;
        
        for( uint8_t j = cluster_boundaries->length - 1; j >= 0; j-- )
        {
            uint8_t label = cluster_boundaries->list[j].label;
            double lower_boundary = band_list->band[i].lower_boundary;
            if( label > 0 )
            {
                if( cluster_boundaries->list[j].value >= lower_boundary)
                {
                    gaussian1d_t new_gaussian = getGaussian1dFrom2dY( &model->gmm.cluster[label].gaussian_out );
                    /* Copy if band gaussian is not initialized */
                    if( !band_gaussian.std_dev )
                    {
                        band_gaussian.mean = new_gaussian.mean;
                        band_gaussian.std_dev = new_gaussian.std_dev;
                    }
                    /* Combine if not */
                    else
                        mulGaussian1d( &band_gaussian, &new_gaussian, &band_gaussian );
                }
            }
            else if( band_gaussian.std_dev )
            {
                gaussian1d_t ended_gaussian = getGaussian1dFrom2dY( &model->gmm.cluster[label].gaussian_out ), test_gaussian;
                divGaussian1d( &band_gaussian, &ended_gaussian, &test_gaussian );
                if( test_gaussian.mean < lower_boundary )
                    bump_to_next_band = true;
                else
                {
                    band_gaussian.mean = test_gaussian.mean;
                    band_gaussian.std_dev = test_gaussian.std_dev;
                }
            }
        }
        band_list->band[i].true_center.b = band_gaussian.mean;
    }
}

uint8_t FindBestClusterPSM( psm_t * model )
{
    uint8_t best_cluster_id = 0;
    double best_cluster_weight = 0.;
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
    {
        gaussian_mixture_cluster_t * cluster = &model->gmm.cluster[i];
        GMMFunctions.Cluster.Weigh( cluster );
        if( cluster->weight > best_cluster_weight )
        {
            best_cluster_weight = cluster->weight;
            best_cluster_id = i;
        }
    }
    return best_cluster_id;
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
