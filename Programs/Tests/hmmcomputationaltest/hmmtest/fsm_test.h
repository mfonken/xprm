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

typedef enum
{
    I = 0,
    O
} OBSERVATAION;

static OBSERVATAION observations[]
{
    I, I, I, I, I,
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

static double initial[2] =
{
    0.2, 0.8
};

static void InitTest( hidden_markov_model_t * hmm )
{
    hmm->A.probabilities = spoof_map;
    hmm->B = spoof_omap;
    

    
    hmm->T = 0;
    hmm->N = 2;
    hmm->M = 2;
    
    hmm->p[0] = initial[0];
    hmm->p[1] = initial[1];
}

static void UpdateFSM( fsm_system_t * fsm )
{
}

#endif /* fsm_test_h */
