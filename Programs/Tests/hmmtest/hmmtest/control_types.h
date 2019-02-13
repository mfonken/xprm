//
//  control_types.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_types_h
#define control_types_h

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "vector_matrix.h"
#include "kumaraswamy.h"

#include "state_control/state_machine_utility.h"

#define NUM_STATES              10
#define NUM_OBSERVATION_SYMBOLS 5 // Should be max number of clusters in GMM
#define MAX_OBSERVATIONS        10 // Length of history

#define MAX_DISTANCE 100
#define MAX_CLUSTERS 5
#define MAX_ERROR 1e-5
#define MIN_MAHALANOBIS_DISTANCE_SQ 1
#define MAX_MAHALANOBIS_SQ_FOR_UPDATE 9
#define SMALL_VALUE_ERROR_OFFSET 1e-4

#define MAX(A,B) (A>B?A:B)

#define ALPHA 0.5
#define BETA 4.

#define UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT 30.
#define SAFE_EXPONENT_MAX_VALUE 2.35385266837019985408e17f

static float safe_exp(double x)
{
    if (x < -UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
        return 0.0f;
    else if (x > UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
        return SAFE_EXPONENT_MAX_VALUE;
    else
        return exp(x);
}

typedef struct
{
    vec2 mean;
    mat2x2 covariance;
} gaussian2d_t;

typedef struct
{
    gaussian2d_t
        gaussian_in,
        gaussian_out;
    double
        mahalanobis_sq,
        log_gaussian_norm_factor,
        probability_condition_input,
        score;
    mat2x2
        inv_covariance_in,
        llt_in;
} gaussian_mixture_cluster_t;

typedef struct
{
    gaussian_mixture_cluster_t
        cluster[MAX_CLUSTERS];
    uint8_t
        num_clusters;
    double
        initial_variance;
    vec2
        max_out,
        min_out;
} gaussian_mixture_model_t;

typedef struct
{
    double map[NUM_OBSERVATION_SYMBOLS][NUM_STATES];
    uint8_t num_observation_symbols, num_states;
} observation_matrix_t;

typedef struct
{
    double                  p[NUM_STATES];       // Initial probabilities
    bayesian_system_t       A;                   // Transition matrix
    observation_matrix_t    B;                   // Observation matrix
    double                  O[MAX_OBSERVATIONS]; // Observation sequence
    double                  K[NUM_STATES];       // Kumaraswamy boundaries
    
    uint8_t
        * T, // Number of observations
        * N, // Number of states
        * M; // Number of observation symbols
    uint8_t
        current_state;
} hidden_markov_model_t;

typedef struct
{
    gaussian_mixture_model_t gmm;
    hidden_markov_model_t hmm;
    kumaraswamy_t kumaraswamy;
} pcr_state_prediction_model_t;

void UpdateCovarianceWithWeight( vec2 * new_val, gaussian2d_t * gaussian, double weight )
{
    vec2 delta_mean;
    vec2SubVec2( new_val, &gaussian->mean, &delta_mean );
    scalarMulVec2( weight, &delta_mean, &gaussian->mean );
    
    double delta_mean_factor = ( delta_mean.a * delta_mean.a ) + ( delta_mean.b * delta_mean.b );
    mat2x2 covariance_delta_factor, unweighted_covariance_factor =
    { delta_mean_factor, SMALL_VALUE_ERROR_OFFSET, SMALL_VALUE_ERROR_OFFSET, SMALL_VALUE_ERROR_OFFSET };
    mat2x2SubMat2x2( &unweighted_covariance_factor, &gaussian->covariance, &covariance_delta_factor );
    scalarMulMat2x2( weight, &covariance_delta_factor, &covariance_delta_factor );
    mat2x2AddMat2x2( &gaussian->covariance, &covariance_delta_factor, &gaussian->covariance );
}

#endif /* control_types_h */
