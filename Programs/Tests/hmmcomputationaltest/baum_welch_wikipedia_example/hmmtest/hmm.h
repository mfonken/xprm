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

#ifdef __cplusplus
extern "C" {
#endif

#include "fsm.h"
    
void addToList(double v);
void searchListCombination(double v);

void   InitializeHMM(                hidden_markov_model_t * );
void   UpdateObservationMatrixHMM(   hidden_markov_model_t *, uint8_t );
void   BaumWelchObservationSolveHMM( hidden_markov_model_t *, uint8_t );
void   BaumWelchTransitionSolveHMM( hidden_markov_model_t * );
void   BaumWelchSolveHMM( hidden_markov_model_t *, uint8_t );

typedef struct
{
    void   (*Initialize)(                hidden_markov_model_t * );
    void   (*UpdateObservationMatrix)(   hidden_markov_model_t *, uint8_t );
    void   (*BaumWelchObservationSolve)( hidden_markov_model_t *, uint8_t );
    void   (*BaumWelchTransitionSolve)( hidden_markov_model_t * );
    void   (*BaumWelchSolve)( hidden_markov_model_t *, uint8_t );
} hidden_markov_model_function_t;

static const hidden_markov_model_function_t HMMFunctions =
{
    .Initialize = InitializeHMM,
    .UpdateObservationMatrix = UpdateObservationMatrixHMM,
    .BaumWelchObservationSolve = BaumWelchObservationSolveHMM,
    .BaumWelchTransitionSolve = BaumWelchTransitionSolveHMM,
    .BaumWelchSolve = BaumWelchSolveHMM
};
    
#ifdef __cplusplus
}
#endif

#endif /* hmm_h */
