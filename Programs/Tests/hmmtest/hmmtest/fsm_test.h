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
    RAIN = 0,
    DRY
} OBSERVATAION;

static observation_buffer_t spoof_observataions;

static band_list_t spoof_bands =
{
    NUM_STATE_GROUPS,
    {0}
};

static fsm_map_t spoof_map =
{
    {
        { 0.3, 0.7 },
        { 0.2, 0.8 }
    },
    NUM_STATES
};

static observation_matrix_t spoof_omap =
{
    {
        { 0.6, 0.4 },
        { 0.4, 0.6 }
    },
    NUM_OBSERVATION_SYMBOLS
};

static void InitTest( hidden_markov_model_t * hmm )
{
    hmm->A.probabilities = spoof_map;
    hmm->B = spoof_omap;
    
    addToObservationBuffer(&hmm->O , RAIN);
    addToObservationBuffer(&hmm->O , DRY);
    
    hmm->T = 2;
    hmm->N = 2;
    hmm->M = hmm->O.next;
    
    hmm->p[0] = 0.4;
    hmm->p[1] = 0.6;
    
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
