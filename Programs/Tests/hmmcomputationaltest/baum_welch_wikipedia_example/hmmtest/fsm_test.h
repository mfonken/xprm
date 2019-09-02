//
//  fsm_test.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/22/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef fsm_test_h
#define fsm_test_h

#include "hmm.h"

#ifdef SPOOF
typedef enum
{
    I = 0,
    O
} OBSERVATAION;

static OBSERVATAION observations[]
{
    I, I, I, I,// I,
    O, O, I
    , I, I
};

static observation_buffer_t spoof_observataions;

static band_list_t spoof_bands =
{
    NUM_STATE_GROUPS,
    {0}
};

static fsm_map_t spoof_map =
{
    {
        { 0.5, 0.5 },
        { 0.3, 0.7 }
    },
    NUM_STATES
};

static observation_matrix_t spoof_omap =
{
    {
        { 0.3, 0.7 },
        { 0.8, 0.2 }
    },
    0,
    NUM_OBSERVATION_SYMBOLS
};

static double initial[NUM_STATES] =
{
    0.2, 0.8
};
#else
typedef enum
{
    ZERO = 0,
    ONE,
    TWO,
    THREE,
    FOUR
} OBSERVATAION;

static OBSERVATAION observations[]
{
//    ONE, ONE, ONE, ONE, TWO, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE, ONE
    ONE, ZERO, THREE, TWO, FOUR, ONE, ZERO, THREE, TWO, FOUR, ONE, ZERO, THREE, TWO, FOUR, ONE, ZERO, THREE, TWO, FOUR
//    ZERO, ZERO, ZERO, ONE, ONE, ONE, TWO, TWO, TWO, THREE, THREE, THREE, FOUR, FOUR, FOUR, THREE, THREE, TWO, TWO, ONE, ONE, ZERO, ZERO
};

static observation_buffer_t spoof_observataions;

static band_list_t spoof_bands =
{
    NUM_STATE_GROUPS,
    {0}
};

static fsm_map_t spoof_map =
{
    {0},
    NUM_STATE_GROUPS
};

static observation_matrix_t spoof_omap =
{
    {0},
    {0},
    NUM_STATE_GROUPS
};

static double initial[NUM_STATES] =
{0};
#endif

static void InitTest( hidden_markov_model_t * hmm )
{
    hmm->T = 0;
    hmm->N = NUM_STATE_GROUPS;
    hmm->M = NUM_OBSERVATION_SYMBOLS;
#ifdef SPOOF    
    hmm->A.probabilities = spoof_map;
    hmm->B = spoof_omap;
    hmm->p[0] = initial[0];
    hmm->p[1] = initial[1];
#else
    double beta = NUM_STATE_GROUPS + 1;
    double half_beta_step = 1 / ( beta );
    
    double row_sum1[NUM_STATE_GROUPS] = {0.};
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        double alpha = (double)i+1;
        double x = half_beta_step;
        for( uint8_t j = 0; j < NUM_STATE_GROUPS; j++, x += 1 / beta )
        {
            double Kij = alpha * beta * pow( x, (alpha - 1) ) * pow( 1 - pow( x, alpha ), beta - 1 );
            hmm->A.probabilities.map[i][j] = Kij;
            row_sum1[i] += Kij;
        }
    }
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
        for( uint8_t j = 0; j < NUM_STATE_GROUPS; j++ )
            hmm->A.probabilities.map[i][j] /= row_sum1[i];
    
    beta = NUM_OBSERVATION_SYMBOLS + 1;
    half_beta_step = 1 / ( 2 * beta );
    
    double row_sum2[NUM_STATE_GROUPS] = {0.};
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
    {
        double alpha = (double)i+1;
        double x = half_beta_step;
        
        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++, x += 1 / beta )
        {
            double Kij = alpha * beta * pow( x, (alpha - 1) ) * pow( 1 - pow( x, alpha ), beta - 1 );
            hmm->B.expected[i][j] = Kij;
            row_sum2[i] += Kij;
        }
    }
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
            hmm->B.expected[i][j] /= row_sum2[i];

    double v = 1./(double)NUM_STATE_GROUPS;
    for(uint8_t i = 0; i < NUM_STATE_GROUPS; i++)
        hmm->p[i] = v;
#endif
}

static void UpdateFSM( fsm_system_t * fsm )
{
}

#endif /* fsm_test_h */
