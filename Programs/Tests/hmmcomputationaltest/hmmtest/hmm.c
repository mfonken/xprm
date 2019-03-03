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
        for(uint8_t j = i+1; j < listi; j++)
        {
            b = list[j];
            for(uint8_t k = j+1; k < listi; k++)
            {
                c = list[k];
//                e = a + b + c;
//                printf("%.4f + %.4f + %.4f = %.4f", a,b,c,e);
//                if( e >= v - ERROR && e <= v + ERROR)
//                    printf(" found");
//                printf("\n");
                for(uint8_t l = k+1; l < listi; l++)
                {
                    d = list[l];
                    e = a + b + c + d;
                    if( e >= v - ERROR && e <= v + ERROR)
                    {
                        printf("%.4f + %.4f + %.4f + %.4f = %.4f\n", a,b,c,d,e);
//                        printf(" found");
                    }
//                    printf("\n");
                }
            }
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
//    /* Observation matrix update */
//    uint8_t o = model->O.data[t];
//    double current_gamma;
//    for( uint8_t i = 0; i < NUM_STATES; i++ )
//    {
//        current_gamma = model->G[i];
//        model->B.observed[i][o] += current_gamma;
//        model->B.gamma[i] += current_gamma;
//    }
//    for( uint8_t i = 0; i < NUM_STATES; i++ )
//    {
//        current_gamma = model->B.gamma[i];
//        double row_sum = 0.;
//        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//        {
//            model->B.expected[i][j] = ZDIV( model->B.observed[i][j], current_gamma );
//            row_sum += model->B.expected[i][j];
//        }
//        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//        {
//            model->B.expected[i][j] /= row_sum;
//        }
//    }
}

void UpdateExpectationMatrixMaximums( hidden_markov_model_t * model )
{
    for( uint8_t k = 0; k < NUM_OBSERVATION_SYMBOLS; k++ )
    {
        for( uint8_t l = 0; l < NUM_OBSERVATION_SYMBOLS; l++ )
        {
            double element_max = model->Em[k][l][0];
            for( uint8_t ii = 1; ii < NUM_STATES; ii++ )
            {
                if( model->Em[k][l][ii] > element_max)
                {
                    element_max = model->Em[k][l][ii];
                }
            }
            model->Em[k][l][NUM_STATES] = element_max;
        }
    }
}

#define EXP  0
#define ST   1
void BaumWelchTransitionSolveHMM( hidden_markov_model_t * model )
{
    /* Expectation matrix update */
    memset( model->beta, 0., sizeof(model->beta) );
    double state_expectation = 0.;
    memset( &model->G, 0, sizeof(model->G));
    /* Update state expectation matrix */
    observation_i k = model->O.prev, l = model->O.curr;
    printf("%s,%s  ", k?"E":"N", l?"E":"N");
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            double a = model->A.probabilities.map[i][j],
            b = model->B.expected[j][l],
            c = model->p[i],
            d = model->B.expected[i][k];
            state_expectation = a * b * c * d;
//            printf("[%d][%d] a:%.3f b:%.3f c:%.3f d:%.3f e:%.3f\n", i, j, a, b, c, d, state_expectation);
            model->Es[k][l][i][j] = state_expectation;
            printf("%.4f%s", model->Es[k][l][i][j], i&&j?" ":",");
            
            addToList(model->Es[k][l][i][j]);
        }
    }
    /* Update maximum expectation matarix */
//    printf("Es > Em:\n");
    bool diff = ( k != l );
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        double max = 0.;
        // Cycle Es row j
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            if( diff && i == j ) continue;
            if( model->Es[k][l][j][i] > max )
            {
//                printf("max < Es[%d][%d][%d][%d]\n",k,l,j,i);
                max = model->Es[k][l][j][i];
            }
        }
        // Cycle Es col j
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            if( diff && i == j ) continue;
            if( model->Es[k][l][i][j] > max )
            {
//                printf("max < Es[%d][%d][%d][%d]\n",k,l,i,j);
                max = model->Es[k][l][i][j];
            }
        }
//        printf("Em[%d][%d][%d] < max\n", k,l,j);
        model->Em[k][l][j] = max;
    }
    
    /* Update total maximum across state maximums in maximum expectation matrix */
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
        {
            double element_max = 0.;
            if( j == l )
            {
                element_max = model->Em[i][j][0];
                for( uint8_t ii = 1; ii < NUM_STATES; ii++ )
                {
                    if( model->Em[i][j][ii] > element_max)
                    {
                        element_max = model->Em[i][j][ii];
                    }
                }
                model->Em[i][j][NUM_STATES] = element_max;
            }
            else
            {
                for( uint8_t ii = 0; ii < NUM_STATES; ii++ )
                {
                    for( uint8_t jj = 0; jj < NUM_STATES; jj++ )
                    {
                        if( model->Es[k][l][ii][jj] > element_max)
                        {
                            element_max = model->Es[k][l][ii][jj];
                        }
                    }
                }
                model->Em[k][l][NUM_STATES] = element_max;
            }
        }
    }
    printf("%.4f", model->Em[k][l][NUM_STATES]);
    
    /* Update gamma expectation mtarix */
//    printf("Em > G:\n");
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
        {
            if( k == j || l == j )
            {
                model->G[j][i] = model->Em[k][l][i];
                model->Gc[j][i] += model->G[j][i];
                model->Gm[j][i] += model->Em[k][l][NUM_STATES];
//                if( j == EXP && i == ST )
                    printf(" <%d,%s|%.4f>",i,j?"E":"N",model->G[j][i]);
            }
        }
        
//        printf(" G%d:%.3f ", i, model->Em[k][l][i]);
    }
//    printf(" Gm:%.3f\n", model->Em[k][l][NUM_STATES]);
    printf("\n");
    
//    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
//    {
//        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//        {
//            double element_max = model->Em[i][j][0];
//            for( uint8_t ii = 1; ii < NUM_STATES; ii++ )
//            {
//                if( model->Em[i][j][ii] > element_max)
//                {
//                    element_max = model->Em[i][j][ii];
//                }
//            }
//            model->Em[i][j][NUM_STATES] = element_max;
//        }
//    }
//    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
//    {
//        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//        {
//            for( uint8_t ii = 0; ii < NUM_STATES; ii++ )
//            {
//                for( uint8_t jj = 0; jj < NUM_STATES; jj++ )
//                {
//                    double element_max = 0.;
//                    for( uint8_t ii = 0; ii < NUM_STATES; ii++ )
//                    {
//                        if( model->Es[i][j][ii][jj] > element_max)
//                        {
//                            element_max = model->Es[i][j][ii][jj];
//                        }
//                    }
//                    model->Em[i][j][NUM_STATES] = element_max;
//                }
//            }
//        }
//    }
    
//        for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//        {
//            double max = 0.;
//            // Cycle Em row j
//            for( uint8_t ii = 0; ii < NUM_OBSERVATION_SYMBOLS; ii++ )
//            {
//                if( model->Em[j][ii][i] > max )
//                    max = model->Em[j][ii][i];
//            }
//            // Cycle Em col j
//            for( uint8_t ii = 0; ii < NUM_OBSERVATION_SYMBOLS; ii++ )
//            {
//                if( model->Em[ii][j][i] > max )
//                    max = model->Em[ii][j][i];
//            }
//            model->G[j][i] = max;
//            model->Gc[j][i] += max;
//        }
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
