//
//  control_structures.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_structures_h
#define control_structures_h

#define _USE_UPDATED_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "control_types_test.h"

/* System self-diagnostic state control type */
typedef struct
{
    state_t         state;
    state_t         prev;
    state_t         next;
    uint8_t         selection_index;
    stability_t     stability;
    fsm_map_t       probabilities;
} fsm_system_t;
    
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
    double
        initial_variance;
    vec2
        min_in,
        max_in,
        max_out,
        min_out;
} gaussian_mixture_model_t;

typedef struct
{
    double expected[NUM_STATES][NUM_OBSERVATION_SYMBOLS];
//    double best[NUM_OBSERVATION_SYMBOLS];
    double gamma[NUM_STATES];
    uint8_t num_observation_symbols;
} observation_matrix_t;
 
typedef double state_expectation_element_t[NUM_STATES][NUM_STATES];
typedef state_expectation_element_t state_expectation_matrix_t[NUM_OBSERVATION_SYMBOLS][NUM_OBSERVATION_SYMBOLS];

typedef double expectation_element_t[NUM_STATES+1];
typedef expectation_element_t expectation_matrix_t[NUM_OBSERVATION_SYMBOLS];
    
typedef double expectation_vector_t[NUM_OBSERVATION_SYMBOLS][NUM_STATES+1];

#ifdef _USE_UPDATED_
#define GAMMA_VALUE_INDEX 1
#define GAMMA
    typedef double gamma_element_t[NUM_OBSERVATION_SYMBOLS][NUM_STATES];
    typedef struct
    {
        gamma_element_t
        value,
        cumulative_value,
        cumulative_maximum;
        double maximum[NUM_OBSERVATION_SYMBOLS];
    } gamma_matrix_t;
    
#else
typedef double gamma_matrix_t[NUM_OBSERVATION_SYMBOLS][NUM_STATES];
#endif
    
typedef struct
{
    double                  p[NUM_STATES];       // Initial probabilities
    fsm_system_t            A;                   // Transition matrix
    observation_matrix_t    B;                   // Observation matrix
    observation_buffer_t    O;                   // Observation sequence
    
    state_expectation_matrix_t  Es;             // State expectation matrix
    expectation_matrix_t    Em;                 // Maximum expectation matrix
    expectation_vector_t    Ev;
    expectation_vector_t    Ec;
    gamma_matrix_t          G;                  // Gamma expectation matrix
    gamma_matrix_t          Gc;                 // Gamma expectation cumulator matrix
    gamma_matrix_t          Gm;                 // Gamma expectation cumulator matrix
    
    double                  K[NUM_STATES];       // Kumaraswamy boundaries
    double                  alpha[NUM_STATES];       // Forward solve vector
    double                  beta[NUM_STATES];       // Backward solve vector
    
    uint8_t
        T, // Number of observations
        N, // Number of states
        M; // Number of observation symbols
    uint8_t
        best_state;
    double
        best_confidence;
} hidden_markov_model_t;

//typedef struct
//{ /* Predictive State Model */
//    gaussian_mixture_model_t gmm;
//    hidden_markov_model_t hmm;
//    kumaraswamy_t kumaraswamy;
//    band_list_t state_bands;
//    double
//        previous_thresh,
//        proposed_thresh,
//        proposed_nu,
//        proposed_primary_id,
//        proposed_secondary_id;
//} psm_t;
    
#ifdef __cplusplus
}
#endif

#endif /* control_structures_h */
