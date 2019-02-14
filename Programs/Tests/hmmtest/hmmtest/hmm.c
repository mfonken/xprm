//
//  hmm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "hmm.h"

void InitializeHMM( hidden_markov_model_t * model, double nu )
{
    BayesianFunctions.Sys.Initialize( &model->A );
    
    memset( &model->B, 0, sizeof(observation_matrix_t) );
}

double ForwardRecursionHMM( hidden_markov_model_t * model, uint8_t j, uint8_t t )
{
    if( t == 0 ) return  model->p[1] * model->B.map[0][j];
    double forward_sum = 0.;
    for( uint8_t i = 0; i < *model->N; i++ )
        forward_sum += HMMFunctions.ForwardRecursion( model, i, t - 1 ) * model->A.probabilities.map[j][i];
    return forward_sum * model->B.map[j][t];
}

double ForwardSolveHMM( hidden_markov_model_t * model )
{
    double pr_now_given_model = 0.;
    for( uint8_t i = 0; i < *model->N; i++ )
        pr_now_given_model += HMMFunctions.ForwardRecursion( model, i, *model->T );
    return pr_now_given_model;
}

void UpdateHMM( hidden_markov_model_t * model )
{
    double best = HMMFunctions.ForwardSolve( model );
}
