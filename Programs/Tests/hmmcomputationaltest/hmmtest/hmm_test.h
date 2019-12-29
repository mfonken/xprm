//
//  fsm_test.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/22/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef hmm_test_h
#define hmm_test_h

#include "hmm.h"

#ifdef DATA_SET_1
#include "data_set_1.h"
#elif DATA_SET_2
#include "data_set_2.h"
#else
#error "No data set specified!"
#endif

static void InitTest( hidden_markov_model_t * hmm )
{
#ifdef SPOOF    
    memcpy(&hmm->A, &spoof_map, sizeof(transition_matrix_t));
    memcpy(&hmm->B, &spoof_omap, sizeof(observation_matrix_t));
    memcpy(&hmm->pi, initial, sizeof(state_vector_t));
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
            hmm->A[i][j] = Kij;
            row_sum1[i] += Kij;
        }
    }
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
        for( uint8_t j = 0; j < NUM_STATE_GROUPS; j++ )
            hmm->A[i][j] /= row_sum1[i];
    
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
            hmm->B[i][j] = Kij;
            row_sum2[i] += Kij;
        }
    }
    for( uint8_t i = 0; i < NUM_STATE_GROUPS; i++ )
        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
            hmm->B[i][j] /= row_sum2[i];

    double v = 1./(double)NUM_STATE_GROUPS;
    for(uint8_t i = 0; i < NUM_STATE_GROUPS; i++)
        hmm->pi[i] = v;
#endif
}

#endif /* hmm_test_h */
