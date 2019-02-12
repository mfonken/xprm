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


void ScoreGaussian( gaussian2d_t * g )
{
    
}


void InitializeHMM( double nu )
{
    BayesianFunctions.Map
}


void UpdateHMMObservationMatrix( observation_matrix_t * matrix)
{
    for( all clusters )
        ScoreGaussianCluster
}

void ReportHMMObservation( double o )
{
    UpdateHMMObservationMatrix();
}

void UpdateHMMTransitionMatrix( hidden_markov_model_t * model, double nu )
{
    // Kumaraswamy state probabilities
    double pr[NUM_STATES];
    GetKumaraswamyVector( &model->kumaraswamy, nu, pr, model->K, NUM_STATES );
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        model->A[model->current_state][i] = pr[i]; // Replace with current BayeSM implementation
}

double HMMForwardRecursion( hidden_markov_model_t * model, uint8_t j, uint8_t t )
{
    if( t == 0 ) return  model->p[1] * model->B.map[0][j];
    double forward_sum = 0.;
    for( uint8_t i = 0; i < model->N; i++ )
        forward_sum += HMMForwardRecursion( model, i, t - 1 ) * model->A.probabilities.map[j][i];
    return forward_sum * model->B.map[j][t];
}

double HMMForwardSolve( hidden_markov_model_t * model )
{
    double pr_now_given_model = 0.;
    for( uint8_t i = 0; i < model->N; i++ )
        pr_now_given_model += HMMForwardRecursion( model, i, model->T );
    return pr_now_given_model;
}

void UpdateHMM()
{
    UpdateHMMTransitionMatrix
    UpdateHMMObservationMatrix
    HMMForwardSolve
    
    result should push threshold (PID and or target filter) towards best cluster mean
}

#endif /* hmm_h */
