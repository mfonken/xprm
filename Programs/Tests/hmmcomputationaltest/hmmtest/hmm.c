//
//  hmm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//
#include "hmm.h"

#define HMM_ERROR_PADDING 0.01
#define SOFTEN(X) (X==1?(1-HMM_ERROR_PADDING):(X==0?HMM_ERROR_PADDING:X))

void InitializeHMM( hidden_markov_model_t * model )
{
    memset( model, 0, sizeof(hidden_markov_model_t) );
}

//uint8_t ReportObservationToHMM( hidden_markov_model_t * model, observation_symbol_t o )
//{
//    return PushToObservationBuffer( &model->O, o );
//}

double UpdateAllHMM( hidden_markov_model_t * model )
{
    HMMFunctions.Update.Alpha(  model );
    HMMFunctions.Update.Beta(   model );
    HMMFunctions.Update.Gamma(  model );
    HMMFunctions.Update.Xi(     model );
    return HMMFunctions.Update.Probability( model );
}

void UpdateAlphaHMM( hidden_markov_model_t * model )
{
    memset( model->alpha, 0., sizeof(model->alpha) );
    uint8_t T = model->O.length;
    double o = GetIndexObservationBuffer( &model->O, 0 ), p;
    
    /* 1. α_i(0) = π_i x B_i(y_0) */
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        model->alpha[0][i] = model->pi[i] * GetProbabilityFromEmission( &model->B[i], o );
    
    /*  2. α_i(t) = B_i(y_(t)) x ∑_{j=0}^N ( α_j(t-1) x A_{ji} ) */
    for( uint8_t t = 1; t < T; t++ )
    {
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            double sum = 0;
            for( uint8_t j = 0; j < NUM_STATES; j++ )
                sum += model->alpha[t - 1][j] * model->A[j][i];
            o = GetIndexObservationBuffer( &model->O, t );
            p = GetProbabilityFromEmission( &model->B[i], o );
            model->alpha[t][i] = sum * p;
        }
    }
}

//double UpdateAlphaRecursiveHMM( hidden_markov_model_t * model, uint8_t t, uint8_t i )
//{
//    printf("AR[%d]: %d|%d\n", ar++, t, i);
//    double o, p, sum = 0.;
//    
//    if( t == 0 ) sum = model->pi[i];
//    else
//        for( uint8_t j = 0; j < NUM_STATES; j++ )
//            sum += UpdateAlphaRecursiveHMM( model, t - 1, j ) * model->A[j][i];
//    o = GetIndexObservationBuffer( &model->O, t );
//    p = GetProbabilityFromEmission( &model->B[i], o );
//    return sum * p;
//}
//
//void UpdateAlphaRecursiveBaseHMM( hidden_markov_model_t * model, uint8_t t )
//{
//    memset( model->alpha, 0., sizeof(model->alpha) );
//    for( uint8_t i = 0; i < NUM_STATES; i++ )
//        model->alpha[t][i] = UpdateAlphaRecursiveHMM( model, t, i );
//}

void UpdateBetaHMM(hidden_markov_model_t * model )
{
    memset( model->beta, 0., sizeof(model->beta) );
    uint8_t T = model->O.length;
    double o, p = 0.;
    
    /* 1. β_i(T-1) = 1 */
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        model->beta[T - 1][i] = 1.;
    
    /* 2. β_i(t) = ∑_{j=1}^N ( β_j(t+1) x A_{ij} x B_j(y_{t+1}) ) */
    for( int8_t t = T - 2; t >= 0; --t)
    {
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            double sum = 0;
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                o = GetIndexObservationBuffer( &model->O, t + 1 );
                p = GetProbabilityFromEmission( &model->B[j], o );
                sum += model->beta[t + 1][j] * model->A[i][j] * p;
            }
            model->beta[t][i] = sum;
        }
    }
}

void UpdateGammaHMM(hidden_markov_model_t * model )
{
    memset( model->gamma, 0., sizeof(model->gamma) );
    uint8_t T = model->O.length;
    
    /* γ_i(t) = ( α_i(t) x β_i(t) ) / ( ∑_{j=1)^N ( α_j(t) x β_j(t) ) ) */
    for( uint8_t t = 0; t < T; t++ )
    {
        double sum = 0;
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            model->gamma[t][i] = model->alpha[t][i] * model->beta[t][i];
            sum += model->gamma[t][i];
        }
        for( uint8_t i = 0; i < NUM_STATES; i++ )
            model->gamma[t][i] /= sum;
    }
}

void UpdateXiHMM( hidden_markov_model_t * model )
{
    memset( model->xi, 0., sizeof(model->xi) );
    uint8_t T = model->O.length;
    double o, p;
    
    /* ξ_{ij}(t) = ( α_i(t) x A_{ij} x β_j(t+1) x B_j(y_{t+1}) ) / ( ∑_{i=1}^N ∑_{j=1)^N ( α_i(t) x A_{ij} x β_j(t+1) x B_j(y_{t+1}) ) ) */
    for( uint8_t t = 0; t < T; t++ )
    {
        double sum = 0;
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                o = GetIndexObservationBuffer( &model->O, t + 1 );
                p = GetProbabilityFromEmission( &model->B[j], o );
                model->xi[i][t][j] = model->alpha[t][i] * model->A[i][j] * model->beta[t + 1][j] * p;
                sum += model->xi[i][t][j];
            }
        }
        for( uint8_t i = 0; i < NUM_STATES; i++ )
            for( uint8_t j = 0; j < NUM_STATES; j++ )
                model->xi[i][t][j] /= sum;
    }
}

double UpdateProbabilityHMM( hidden_markov_model_t * model )
{
    double sum = 0;
    uint8_t T = model->O.length;
    
    /* P(T) = ∑_{i=1}^N α_i(T) */
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        sum += model->alpha[T - 1][i];
    return sum;
}

mat2x2 ComputeObservationAAT( observation_t A )
{
    double aa = A.a * A.a,
    ab = A.a * A.b,
    bb = A.b * A.b;
    return (mat2x2){ aa, ab, ab, bb };
}

void UpdateInitialProbabilitiesHMM( hidden_markov_model_t * model )
{
    /* π_i^* = γ_i(0) */
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        model->pi[i] = SOFTEN( model->gamma[0][i] );
}

void UpdateTransitionProbabilitiesHMM( hidden_markov_model_t * model )
{
    uint8_t T = model->O.length;
    
    /* A_{ij}^* = ( ∑_{t=1}^{T-1} ξ_{ij}(t) ) / ( ∑_{t=1}^{T-1} γ_i(t) ) */
    double n = 0, d = 0;
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            n = 0; d = 0;
            for( uint8_t t = 0; t < T - 1; t++ )
            {
                n += model->xi[i][t][j];
                d += model->gamma[t][i];
            }
            model->A[i][j] = SOFTEN( n / d );
        }
    }
}

void UpdateEmissionProbabilitiesHMM( hidden_markov_model_t * model )
{
#ifdef HMM_GAUSSIAN_EMISSIONS
    /* P( y_t | s_t=i ) = Ν(nu)( y_t | µ_i, ∑_i ) */
    uint8_t T = model->O.length, o;
    double sum = 0., gamma_sum = 0.;
    double mean_diff = 0.;
//    mat2x2 cov_est = { 0. };
    
    /* µ_i = ( ∑_{t=1}^T γ_i(t) x y(t) ) / ( ∑_{t=1}^T γ_i(t) ) */
    /* ∑_i = ( ∑_{t=1}^T γ_i(t) x [y(t) - µ_i][y(t) - µ_i]^T ) / ( ∑_{t=1}^T γ_i(t) ) */
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        sum = 0.; gamma_sum = 0.;
        for( uint8_t t = 0; t < T; t++ )
        {
            o = GetIndexObservationBuffer( &model->O, t );
            sum += model->gamma[t][i] * o;
            gamma_sum += model->gamma[t][i];
        }
        model->B[i].mean = SOFTEN( sum / gamma_sum );
        
        sum = 0.;
        for( uint8_t t = 0; t < T; t++ )
        {
            o = GetIndexObservationBuffer( &model->O, t );
            mean_diff = (double)o - model->B[i].mean;
            //            cov_est = ComputeEmissionAAT( mean_diff );
            sum += model->gamma[t][i] * mean_diff * mean_diff;// cov_est;
        }
        model->B[i].std_dev = SOFTEN( sum / gamma_sum );
    }
#else
    /* B_i^*(v_k) = ( ∑_{t=1}^{T-1} ( 1_{y_t=v_k} x γ_i(t) ) ) / ( ∑_{t=1}^{T-1} γ_i(t) ) */
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        for( uint8_t k = 0; k < NUM_OBSERVATION_SYMBOLS; ++k)
        {
            n = 0; d = 0;
            for( uint8_t t = 0; t < T; ++t )
            {
                o = GetIndexObservationBuffer( &model->O, t );
                if( o == k )
                    n += model->gamma[t][j];
                d += model->gamma[t][j];
            }
            model->B[j][k] = SOFTEN( n / d );
        }
    }
#endif
}

void BaumWelchSolveHMM( hidden_markov_model_t * model, double DELTA )
{
    double prob_start = HMMFunctions.Update.All( model ),
    prob_end = 0,
    prob_delta = 0;
    
    do
    {
        /* Update initial state probabilities */
        HMMFunctions.Update.Pi( model );
        
        /* Update transition matrix */
        HMMFunctions.Update.A( model );
        
        /* Update observation matrix */
        HMMFunctions.Update.B( model );
        
        prob_end = HMMFunctions.Update.All( model );
        prob_delta = prob_end - prob_start;
        prob_start = prob_end;
    } while (prob_delta > DELTA);
}

void PrintHMM( hidden_markov_model_t * model )
{
    printf("\n");
    printf("A:");
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        printf("[");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
            printf("%.4f%s", model->A[i][j], (j==NUM_STATES-1?"":","));
        printf("]");
    }
    printf("\n");
    printf("B:");
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        printf("[");
        printf("%.4f,%.4f", model->B[i].mean, model->B[i].std_dev);
        printf("]");
    }
    printf("\n");
}
