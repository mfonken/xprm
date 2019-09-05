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
 
    
typedef gaussian1d_t emission_t;
typedef double transition_matrix_t[NUM_STATES][NUM_STATES];
typedef emission_t observation_matrix_t[NUM_STATES];
typedef double state_sequence_matrix[MAX_OBSERVATIONS][NUM_STATES];
    
typedef struct
{
    transition_matrix_t     A;                   // Transition matrix
    observation_matrix_t    B;                   // Observation matrix
    observation_buffer_t    O;                   // Observation sequence
    double                  pi[NUM_STATES];      // Initial state probabilities
    state_sequence_matrix   alpha;               // Forward solve vector
    state_sequence_matrix   beta;                // Backward solve vector
    state_sequence_matrix   gamma;               // Gamma solve vector
    state_sequence_matrix   xi[NUM_STATES];      // Xi solve matrix
} hidden_markov_model_t;

static double GetProbabilityFromEmission( emission_t * e, double v )
{
    return getProbabilityFromGaussian( (gaussian1d_t *)e, v );
}

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
