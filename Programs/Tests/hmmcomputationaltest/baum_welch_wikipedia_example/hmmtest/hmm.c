//
//  hmm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "hmm.h"

uint8_t listi = 0;
double list[1 << 8] = { 0 };

void addToList(double v)
{
    bool has = false;
    for(uint8_t i = 0; i < listi; i++)
        if(v == list[i])
            has = true;
    if(!has)
        list[listi++] = v;
}
#define ERROR 0.0001
void searchListCombination(double v)
{
    double a = 0., b = 0., c = 0., d = 0., e = 0.;
    for(uint8_t i = 0; i < listi; i++)
    {
        a = list[i];
        for(uint8_t j = i; j < listi; j++)
        {
            b = list[j];
            e = a + b ;
            if( e >= v - ERROR && e <= v + ERROR)
                printf("%.4f + %.4f = %.4f\n", a,b,e);
        }
    }
}

void InitializeHMM( hidden_markov_model_t * model )
{
    FSMFunctions.Sys.Initialize( &model->A );
    memset( &model->B, 0, sizeof(observation_matrix_t) );
}


void UpdateObservationMatrixHMM( hidden_markov_model_t * model, uint8_t t )
{
}

static double ForwardBackward( hidden_markov_model_t * model, uint8_t k, uint8_t l, uint8_t i, uint8_t j )
{
    double a = model->A.probabilities.map[i][j],
    b = model->B.expected[j][l],
    c = model->p[i],
    d = model->B.expected[i][k];
    return a * b * c * d;
}

static int count = 0;
void BaumWelchTransitionSolveHMM( hidden_markov_model_t * model )
{
    /* Expectation matrix update */
    memset( model->beta, 0., sizeof(model->beta) );
    memset( model->Ev, 0, sizeof(model->Ev) );
    double state_expectation = 0.;
    memset( &model->G, 0, sizeof(model->G));
    /* Update state expectation matrix */
    observation_i k = model->O.prev, l = model->O.curr;
    printf("%4d: %s,%s  ", ++count,
#ifdef SPOOF
           k?"E":"N", l?"E":"N"
#else
           observation_strings[k], observation_strings[l]
#endif
           );
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            state_expectation = ForwardBackward(model, k, l, i, j);
            model->Es[k][l][i][j] = state_expectation;
            printf("%.4f%s", model->Es[k][l][i][j], i&&j?" ":",");
        }
    }
    printf("\n");
    /* Update maximum expectation matarix */
    bool diff = ( k != l );
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        double curr_max = 0., prev_max = 0., curr = 0.;
        double diag = model->Es[k][l][i][i];
        
        if( !diff )
        {
            curr_max = diag;
        }
        else
        {
            // assume l comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = model->Es[k][l][j][i];
                if( curr > curr_max )
                    curr_max = curr;
            }

            // assume k comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = model->Es[k][l][i][j];
                if( curr > prev_max )
                    prev_max = curr;
            }
        }
        printf("k:%d l:%d i:%d prev:%.4f curr:%.4f\n", k,l,i,prev_max, curr_max);
//        model->G[k][i] = prev_max;
//        model->G[l][i] = curr_max;
        model->Gc[k][i] += prev_max;
        model->Gc[l][i] += curr_max;
        model->Ev[k][NUM_STATES] = MAX( prev_max, model->Ev[k][NUM_STATES] );
        model->Ev[l][NUM_STATES] = MAX( curr_max, model->Ev[l][NUM_STATES] );
    }
#ifdef SPOOF
    printf("[%.4f %.4f %.4f][%.4f %.4f %.4f] | [%.4f %.4f %.4f][%.4f %.4f %.4f]",
           model->G[0][0],model->G[0][1],model->Ev[0][2],
           model->G[1][0],model->G[1][1],model->Ev[1][2],
           model->Gc[0][0],model->Gc[0][1],model->Ec[0][2],
           model->Gc[1][0],model->Gc[1][1],model->Ec[1][2]);
#endif
    /* Update gamma expectation mtarix */
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            model->Gm[i][j] += model->Ev[i][NUM_STATES];
        }
    }
    printf("\n");
}

void BaumWelchObservationSolveHMM( hidden_markov_model_t * model, uint8_t t )
{
//    /* Gamma expectation vector update */
//    memset( model->alpha, 0., sizeof(model->alpha) );
//    double state_probability = 0., cumulated_probability = 0.;
//    for( uint8_t j = 0; j < NUM_STATES; j++ )
//    {
//        state_probability = HMMFunctions.ForwardRecursion( model, j, t ) * HMMFunctions.BackwardRecursion( model, j, t );
//        model->G[j] = state_probability;
//        cumulated_probability += state_probability;
//    }
//    if( cumulated_probability > 0 )
//        for( uint8_t j = 0; j < NUM_STATES; j++ )
//            model->G[j] /= cumulated_probability;
//
//    HMMFunctions.UpdateObservationMatrix( model, t );
}

void BaumWelchTransitionStepHMM( hidden_markov_model_t * model, uint8_t t )
{
    
}

void BaumWelchSolveHMM( hidden_markov_model_t * model, uint8_t t )
{
    HMMFunctions.BaumWelchTransitionSolve( model );
//    HMMFunctions.BaumWelchObservationSolve( model, t );
}
