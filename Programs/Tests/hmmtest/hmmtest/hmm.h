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


double ScoreGaussian( gaussian2d_t * g )
{
    return 0.;
}

void InitializeHMM( double nu )
{
    BayesianFunctions.Map
}

void UpdateHMMObservationMatrix( hidden_markov_model_t * model, double O_new[NUM_OBSERVATION_SYMBOLS] )
{
    for( uint8_t m = 0; m < *model->M; m++ )
        model->B.map[m][model->current_state] = O_new[m]; //ScoreGaussian( &model->gmm.cluster[k].gaussian_out );
}

void ReportHMMObservation( double o )
{
    UpdateHMMObservationMatrix();
}

void UpdateHMMTransitionMatrix( hidden_markov_model_t * model, double S_new[NUM_STATES] )
{
    // Kumaraswamy state probabilities
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        model->A.probabilities.map[model->current_state][i] = S_new[i]; // Replace with current BayeSM implementation
}

double HMMForwardRecursion( hidden_markov_model_t * model, uint8_t j, uint8_t t )
{
    if( t == 0 ) return  model->p[1] * model->B.map[0][j];
    double forward_sum = 0.;
    for( uint8_t i = 0; i < *model->N; i++ )
        forward_sum += HMMForwardRecursion( model, i, t - 1 ) * model->A.probabilities.map[j][i];
    return forward_sum * model->B.map[j][t];
}

double HMMForwardSolve( hidden_markov_model_t * model )
{
    double pr_now_given_model = 0.;
    for( uint8_t i = 0; i < *model->N; i++ )
        pr_now_given_model += HMMForwardRecursion( model, i, *model->T );
    return pr_now_given_model;
}

void UpdateHMM( hidden_markov_model_t * model, double nu )
{
    UpdateHMMTransitionMatrix( model, nu );
    UpdateHMMObservationMatrix( model );
    
    double best = HMMForwardSolve( model );
    
    result should push threshold (PID and or target filter) towards best cluster mean
}

#endif /* hmm_h */
