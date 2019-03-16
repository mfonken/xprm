//
//  control_structures.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_structures_h
#define control_structures_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include "control_types.h"
    
typedef struct
{
    gaussian2d_t
        gaussian_in,
        gaussian_out;
    double
        mahalanobis_sq,
        log_gaussian_norm_factor,
        probability_of_in,
        probability_condition_input,
        score, weight;
    mat2x2
        inv_covariance_in,
        llt_in;
    label_manager_t labels;
    double max_y, min_y;
    uint8_t primary_id, secondary_id;
} gaussian_mixture_cluster_t;

typedef struct
{
    gaussian_mixture_cluster_t *
        cluster[MAX_CLUSTERS];
    uint8_t
        num_clusters,
        selected_cluster;
    vec2
        min_in,
        max_in,
        max_out,
        min_out;
    gaussian_mixture_cluster_t
        cluster_mem[MAX_CLUSTERS];
} gaussian_mixture_model_t;
    
    
typedef struct
{ /* Predictive State Model */
    gaussian_mixture_model_t gmm;
//    hidden_markov_model_t hmm;
//    kumaraswamy_t kumaraswamy;
    band_list_t state_bands;
    double
    previous_thresh,
    proposed_thresh,
    proposed_nu,
    proposed_primary_id,
    proposed_secondary_id;
    uint8_t
    best_state,
    best_cluster_id,
    observation_state,
    current_state;
    double
    best_confidence,
    best_cluster_weight;
} psm_t;

#ifdef __cplusplus
}
#endif

#endif /* control_structures_h */
