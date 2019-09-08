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
    
#include "kumaraswamy.h"

#ifdef __PSM__
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
        double
        max_y,
        min_y,
        timestamp;
        uint8_t
        primary_id,
        secondary_id;
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

    typedef double transition_matrix_t[NUM_STATES][NUM_STATES];
    typedef emission_t observation_matrix_t[NUM_STATES];
    typedef double state_sequence_matrix[MAX_OBSERVATIONS][NUM_STATES];
    typedef double state_vector_t[NUM_STATES];
    
    typedef struct
    {
        transition_matrix_t     A;                   // Transition matrix
        observation_matrix_t    B;                   // Observation matrix
        observation_buffer_t    O;                   // Observation sequence
        state_vector_t          pi;                  // Initial state probabilities
        state_sequence_matrix   alpha;               // Forward solve vector
        state_sequence_matrix   beta;                // Backward solve vector
        state_sequence_matrix   gamma;               // Gamma solve vector
        state_sequence_matrix   xi[NUM_STATES];      // Xi solve matrix
        double                  P;                   // Latest probability
    } hidden_markov_model_t;
    
    static floating_t GetProbabilityFromEmission( emission_t * e, hmm_observation_t v )
    {
#ifdef HMM_2D_EMISSIONS
        return MatVec.Gaussian2D.Probability( (gaussian2d_t *)e, (vec2 *)&v );
#else
        return MatVec.Gaussian1D.Probability( (gaussian1d_t *)e, (double)v );
#endif
    }
#endif
    
    /* System self-diagnostic state control type */
    typedef struct
    {
        state_t         state;
        state_t         prev;
        state_t         next;
        uint8_t         selection_index;
        stability_t     stability;
        transition_matrix_t *P;
    } fsm_system_t;
    
    typedef struct
    { /* Predictive State Model */
#ifdef __PSM__
        gaussian_mixture_model_t gmm;
        hidden_markov_model_t hmm;
        fsm_system_t fsm;
#endif
        kumaraswamy_t kumaraswamy;
        band_list_t state_bands;
        double
        previous_thresh,
        proposed_thresh,
        proposed_nu,
        proposed_avg_den,
        proposed_primary_id,
        proposed_secondary_id;
        uint8_t
        best_state,
        best_cluster_id,
        observation_state;
        state_t
        current_state;
#ifdef HMM_2D_EMISSIONS
        vec2
#else
        double
#endif
        current_observation;
        double
        best_confidence,
        best_cluster_weight,
        state_intervals[NUM_STATE_GROUPS];
    } psm_t;
    
#ifdef __cplusplus
}
#endif

#endif /* control_structures_h */
