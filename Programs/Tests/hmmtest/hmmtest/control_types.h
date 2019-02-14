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

#define MAX_LABELS 10
#define LABEL_MOVING_AVERAGE_MAX_HISTORY 10
#define NULL_LABEL 0xff
#define MIN_LABEL_CONTRIBUTION 0.1

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
    double a, b;
    uint8_t label;
} observation_t;

typedef struct
{
    double average[MAX_LABELS];
    uint8_t count[MAX_LABELS];
} label_manager_t;

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
    label_manager_t labels;
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
    uint8_t num_observation_symbols;
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
{ /* Predictive State Model */
    gaussian_mixture_model_t gmm;
    hidden_markov_model_t hmm;
    kumaraswamy_t kumaraswamy;
} psm_t;

static void UpdateCovarianceWithWeight( vec2 * new_val, gaussian2d_t * gaussian, double weight )
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

static bool ReportLabel( label_manager_t * labels, uint8_t new_label )
{
    if( new_label > MAX_LABELS ) return false;
    labels->count[new_label]++;
    double sum = 0;
    for( uint8_t i = 0; i < MAX_LABELS; i++ )
        sum += (double)labels->count[i];
    for( uint8_t i = 0; i < MAX_LABELS; i++ )
        labels->average[i] = (double)labels->count[i] / sum;
    return true;
}

static void GetTopTwoLabels( label_manager_t * labels, uint8_t * first, uint8_t * second )
{
    *first = 0; *second = 1;
    double first_value = labels->average[0], second_value = labels->average[1], current = 0.;
    for( uint8_t i = 2; i < MAX_LABELS; i++ )
    {
        current = labels->average[i];
        if( current > first_value )
        {
            second_value = first_value;
            *second = *first;
            first_value = current;
            *first = i;
        }
        else if ( current > second_value )
        {
            second_value = current;
            *second = i;
        }
    }
}

static uint8_t GetNumLabels( label_manager_t * labels )
{
    uint8_t num = 0;
    for( uint8_t i = 0; i < MAX_LABELS; i++ )
    {
        if( labels->average[i] > MIN_LABEL_CONTRIBUTION )
            num++;
    }
    return num;
}

#endif /* control_types_h */
