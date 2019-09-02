//
//  hmm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "hmm.h"

#define ERROR_PADDING 0.01
#define SOFTEN(X) (X==1?(1-ERROR_PADDING):(X==0?ERROR_PADDING:X))

void InitializeHMM( hidden_markov_model_t * model )
{
    memset( model, 0, sizeof(hidden_markov_model_t) );
}

double UpdateAllHMM( hidden_markov_model_t * model )
{
    HMMFunctions.Update.Alpha( model );
    HMMFunctions.Update.Beta( model );
    HMMFunctions.Update.Gamma( model );
    HMMFunctions.Update.Xi( model );
    return HMMFunctions.Update.Probability( model );
}

void UpdateAlphaHMM( hidden_markov_model_t * model )
{
    memset( model->alpha, 0., sizeof(model->alpha) );
    uint8_t T = model->O.length, o = GetIndexObservationBuffer( &model->O, 0 );
    
    /* 1. α_i(0) = π_i x B_i(y_0) */
    for (uint8_t i = 0; i < NUM_STATES; ++i)
    {
        model->alpha[0][i] = model->pi[i] * model->B[i][o];
    }
    
    /*  2. α_i(t) = B_i(y_(t)) x ∑_(j=0)^N ( α_j(t-1) x A_{ji} ) */
    for (uint8_t t = 1; t < T; ++t)
    {
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            double sum = 0;
            for (uint8_t j = 0; j < NUM_STATES; ++j)
            {
                sum += model->alpha[t - 1][j] * model->A[j][i];
            }
            o = GetIndexObservationBuffer( &model->O, t );
            model->alpha[t][i] = sum * model->B[i][o];
        }
    }
}

void UpdateBetaHMM(hidden_markov_model_t * model )
{
    memset( model->beta, 0., sizeof(model->beta) );
    uint8_t T = model->O.length, o;
    
    /* 1. β_i(T-1) = 1 */
    for (uint8_t i = 0; i < NUM_STATES; ++i)
    {
        model->beta[T - 1][i] = 1.;
    }
    
    /* 2. β_i(t) = ∑_(j=0)^N ( β_j(t+1) x A_{ij} x B_j(y_{t+1}) ) */
    for (int8_t t = T - 2; t >= 0; --t)
    {
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            double sum = 0;
            for (uint8_t j = 0; j < NUM_STATES; ++j)
            {
                o = GetIndexObservationBuffer( &model->O, t + 1 );
                sum += model->beta[t + 1][j] * model->A[i][j] * model->B[j][o];
            }
            model->beta[t][i] = sum;
        }
    }
}

void UpdateGammaHMM(hidden_markov_model_t * model )
{
    memset( model->gamma, 0., sizeof(model->gamma) );
    uint8_t T = model->O.length;
    
    /* γ_i(t) = ( α_i(t) x β_i(t) ) / ( ∑_{j=0)^N ( α_j(t) x β_j(t) ) ) */
    for (uint8_t t = 0; t < T; ++t)
    {
        double sum = 0;
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            model->gamma[t][i] = model->alpha[t][i] * model->beta[t][i];
            sum += model->gamma[t][i];
        }
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            model->gamma[t][i] /= sum;
        }
    }
}

void UpdateXiHMM( hidden_markov_model_t * model )
{
    memset( model->xi, 0., sizeof(model->xi) );
    uint8_t T = model->O.length, o;
    
    /* ξ_{ij}(t) = ( α_i(t) x A_{ij} x β_j(t+1) x B_j(y_{t+1}) ) / ( ∑_{i=0)^N ∑_{j=0)^N ( α_i(t) x A_{ij} x β_j(t+1) x B_j(y_{t+1}) ) ) */
    for (uint8_t t = 0; t < T; ++t)
    {
        double sum = 0;
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            for (uint8_t j = 0; j < NUM_STATES; ++j)
            {
                o = GetIndexObservationBuffer( &model->O, t + 1 );
                model->xi[i][t][j] = model->alpha[t][i] * model->A[i][j] * model->beta[t + 1][j] * model->B[j][o];
                sum += model->xi[i][t][j];
            }
        }
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            for (uint8_t j = 0; j < NUM_STATES; ++j)
            {
                model->xi[i][t][j] /= sum;
            }
        }
    }
}

double UpdateProbabilityHMM( hidden_markov_model_t * model )
{
    /* P(T) = ∑_{i=0}^N α_i(T) */
    double sum = 0;
    uint8_t T = model->O.length;
    for (uint8_t i = 0; i < NUM_STATES; ++i)
    {
        sum += model->alpha[T - 1][i];
    }
    return sum;
}

void BaumWelchSolveHMM( hidden_markov_model_t * model, double DELTA )
{
    double prob_start = HMMFunctions.Update.All( model );
    double prob_end = 0;
    double prob_delta = 0;

    uint8_t T = model->O.length, o;
    
    do
    {
        /* Update initial state probability
         * π_i^* = γ_i(0)
         */
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            model->pi[i] = SOFTEN( model->gamma[0][i] );
        }
        
        /* Update transition matrix
         * A_{ij}^* = ( ∑_{t=1}^{T-1} ξ_{ij}(t) ) / ( ∑_{t=1}^{T-1} γ_i(t) )
         */
        for (uint8_t i = 0; i < NUM_STATES; ++i)
        {
            for (uint8_t j = 0; j < NUM_STATES; ++j)
            {
                double numerator = 0;
                double denominator = 0;
                for (uint8_t t = 0; t < T - 1; ++t)
                {
                    numerator += model->xi[i][t][j];
                    denominator += model->gamma[t][i];
                }
                model->A[i][j] = SOFTEN( numerator / denominator );
            }
        }
        
        /* Update observation matrix
         * B_i^*(v_k) = ( ∑_{t=1}^{T-1} ( 1_{y_t=v_k} x γ_i(t) ) ) / ( ∑_{t=1}^{T-1} γ_i(t) )
         */
        for (uint8_t j = 0; j < NUM_STATES; ++j)
        {
            for (uint8_t k = 0; k < NUM_OBSERVATION_SYMBOLS; ++k)
            {
                double numerator = 0;
                double denominator = 0;
                for (uint8_t t = 0; t < T; ++t )
                {
                    o = GetIndexObservationBuffer( &model->O, t );
                    if( o == k )
                    {
                        numerator += model->gamma[t][j];
                    }
                    denominator += model->gamma[t][j];
                }
                model->B[j][k] = SOFTEN( numerator / denominator );
            }
        }

        prob_end = HMMFunctions.Update.All( model );
        prob_delta = prob_end - prob_start;
        prob_start = prob_end;

    } while (prob_delta > DELTA);
}

