//
//  data_set_2.h
//  hmmtest
//
//  Created by Matthew Fonken on 12/21/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef data_set_2_h
#define data_set_2_h

#include "control_types.h"

static hmm_observation_t observations[]
{
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12
};

static transition_matrix_t spoof_map =
{
    { 0.7,  0.1,  0.1,  0.1  },
    { 0.15, 0.7,  0.15, 0    },
    { 0,    0.15, 0.7,  0.15 },
    { 0.1,  0.1,  0.1,  0.7  },
};

static observation_matrix_t spoof_omap =
{
    { 2,   2.5 },
    { 4,   2.5 },
    { 6,   2.5 },
    { 8,   2.5 },
    { 10,  2.5 },
};

static double initial[NUM_STATES] =
{
    0.25, 0.25, 0.25, 0.25
};


#endif /* data_set_2_h */
