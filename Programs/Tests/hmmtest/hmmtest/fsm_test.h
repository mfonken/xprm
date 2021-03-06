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

#define THETA_STEP M_PI/20

static kumaraswamy_t dist;
static double theta = 0.;
static double nu = 0.;

static double spoof_intervals[NUM_STATE_GROUPS];

typedef enum
{
    I = 0,
    O
} OBSERVATAION;

static OBSERVATAION observations[10]
{ I, I, I, I, I, O, O, I, I, I };

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

static double initial[2] =
{
    0.2, 0.8
};

static void InitTest( hidden_markov_model_t * hmm )
{
    hmm->A.probabilities = spoof_map;
    hmm->B = spoof_omap;
    

    
    hmm->T = hmm->O.last;
    hmm->N = 2;
    hmm->M = 2;
    
    hmm->p[0] = initial[0];
    hmm->p[1] = initial[1];
    
//    KumaraswamyFunctions.Initialize( &dist, NUM_STATES+1 );
//    double curr = 0, prev = 0;
//    for( uint8_t i = 0; i < spoof_bands.length; i++ )
//    {
//        curr = (double)(1+1)/NUM_STATE_GROUPS;
//        spoof_bands.band[i].lower_boundary = curr;
//        spoof_bands.band[i].true_center = { 5, (curr+prev)/2 };
//        prev = curr;
//    }
}

static void UpdateFSM( fsm_system_t * fsm )
{
//    nu = dist.beta * fabs( sin( theta ) );
//    theta += THETA_STEP;
//    KumaraswamyFunctions.GetVector( &dist, nu, spoof_intervals, &spoof_bands );
    
//    FSMFunctions.Sys.Update( fsm, spoof_intervals );
    
    
    
    
}

#endif /* fsm_test_h */
