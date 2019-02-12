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
//    vec2
//    mean_in,
//    mean_out;
    double
    mahalanobis_sq,
    log_gaussian_norm_factor,
    probability_condition_input,
    score;
    mat2x2
//    covariance_in,
    inv_covariance_in,
//    covariance_in_out,
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
} observation_matrix_t;

typedef struct
{
    double p[NUM_STATES];   // Initial probabilities
    bayesian_system_t A;      // Transition matrix
    observation_matrix_t B; // Observation matrix
    double a[MAX_OBSERVATIONS];
    double O[MAX_OBSERVATIONS]; // Observation sequence
    double K[MAX_STATES]; // Kumaraswamy boundaries
    
    uint8_t
    T, // Number of observations
    N, // Number of states
    M; // Number of observation symbols
    uint8_t
    current_state;
    
    kumaraswamy_t kumaraswamy;
} hidden_markov_model_t;


#endif /* control_types_h */
