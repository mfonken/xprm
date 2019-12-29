//
//  data_set_1.h
//  hmmtest
//
//  Created by Matthew Fonken on 12/21/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef data_set_1_h
#define data_set_1_h

#include "control_types.h"

static hmm_observation_t observations[]
{
    1,1,0,1,0,0,1,0,1,1,0,0,0,1
};

static transition_matrix_t spoof_map =
{
    { 0.3, 0.3,  0.4  },
    { 0.1, 0.45, 0.45 },
    { 0.2, 0.3,  0.5  }
};

static observation_matrix_t spoof_omap =
{
    { 0.99, 0.01 },
    { 0.8,  0.2  },
    { 0.3,  0.7  }
};

static double initial[NUM_STATES] =
{
    0, 0.2, 0.8
};

#endif /* data_set_1_h */
