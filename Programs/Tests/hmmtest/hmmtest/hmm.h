//
//  hmm.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef hmm_h
#define hmm_h

#include <stdio.h>
#include "gmm.h"

void InitializeHMM(         hidden_markov_model_t *, double );
double ForwardRecursionHMM( hidden_markov_model_t *, uint8_t, uint8_t );
double ForwardSolveHMM(     hidden_markov_model_t * );
void UpdateHMM(             hidden_markov_model_t * );

typedef struct
{
    void (*Initialize)(         hidden_markov_model_t *, double );
    double (*ForwardRecursion)( hidden_markov_model_t *, uint8_t, uint8_t );
    double (*ForwardSolve)(     hidden_markov_model_t * );
    void (*Update)(             hidden_markov_model_t * );
} hidden_markov_model_function_t;

static const hidden_markov_model_function_t HMMFunctions =
{
    .Initialize = InitializeHMM,
    .ForwardRecursion = ForwardRecursionHMM,
    .ForwardSolve = ForwardSolveHMM,
    .Update = UpdateHMM
};

#endif /* hmm_h */
