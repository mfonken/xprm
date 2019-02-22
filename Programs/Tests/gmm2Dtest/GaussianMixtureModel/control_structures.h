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
    gaussian_mixture_cluster_t
        cluster[MAX_CLUSTERS];
    uint8_t
        num_clusters,
        selected_cluster;
    vec2
        min_in,
        max_in,
        max_out,
        min_out;
} gaussian_mixture_model_t;

#ifdef __cplusplus
}
#endif

#endif /* control_structures_h */
