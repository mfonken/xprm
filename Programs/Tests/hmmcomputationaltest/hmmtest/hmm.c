
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
            //            for(uint8_t k = j; k < listi; k++)
            //            {
            //                c = list[k];
            ////                e = a + b + c;
            ////                printf("%.4f + %.4f + %.4f = %.4f", a,b,c,e);
            ////                if( e >= v - ERROR && e <= v + ERROR)
            ////                    printf(" found");
            ////                printf("\n");
            //                for(uint8_t l = k; l < listi; l++)
            //                {
            //                    d = list[l];
            //                    e = a + b + c + d;
            //                    if( e >= v - ERROR && e <= v + ERROR)
            //                    {
            //                        printf("%.4f + %.4f + %.4f + %.4f = %.4f\n", a,b,c,d,e);
            ////                        printf(" found");
            //                    }
            ////                    printf("\n");
            //                }
            //            }
        }
    }
}

void InitializeHMM( hidden_markov_model_t * model )
{
    FSMFunctions.Sys.Initialize( &model->A );
    memset( &model->B, 0, sizeof(observation_matrix_t) );
}


void UpdateObservationMatrixHMM(  hidden_markov_model_t * model )
{
    /* Observation matrix update */
    LOG_HMM(HMM_DEBUG, "Observation matrix update:\n");
    LOG_HMM(HMM_DEBUG, "    ");
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        LOG_HMM_BARE(HMM_DEBUG, "  %s    ", observation_strings[i]);
    LOG_HMM_BARE(HMM_DEBUG, "\n");
#ifdef NORMALIZE_BY_STATE
    /* Normalize by state */
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        LOG_HMM(HMM_DEBUG, "%s ", stateString(j));
        double row_sum = 0., row_s = 0.;
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            row_sum += model->G.cumulative_value[i][j];
        }
        
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            if( row_sum != 0. )
            {
                model->B.expected[i][j] = ZDIV( model->G.cumulative_value[i][j], row_sum );
            }
            LOG_HMM_BARE(HMM_DEBUG, "|%.4f|", model->B.expected[i][j]);
            row_s += model->B.expected[i][j];
        }
        LOG_HMM_BARE(HMM_DEBUG, " = %.3f\n", row_s);
    }
#else
    double observation_sums[NUM_OBSERVATION_SYMBOLS] = { 0. };
    /* Normalize by symbol */
    for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
    {
        double row_sum = 0.;
        for( uint8_t i = 0; i < NUM_STATES; i++ )
        {
            row_sum += model->G.cumulative_value[j][i];
        }
        
        if( row_sum != 0. )
        {
            for( uint8_t i = 0; i < NUM_STATES; i++ )
            {
                model->B.expected[j][i] = ZDIV( model->G.cumulative_value[j][i], row_sum );
                //                observation_sums[j] += model->B.expected[j][i];
            }
        }
    }
    
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        LOG_HMM(HMM_DEBUG, "%s ", stateString(j));
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            LOG_HMM_BARE(HMM_DEBUG, "|%.4f|", model->B.expected[i][j]);
            observation_sums[i] += model->B.expected[i][j];
        }
        LOG_HMM_BARE(HMM_DEBUG, "\n");
    }
    LOG_HMM(HMM_DEBUG, "    ");
    for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
    {
        LOG_HMM_BARE(HMM_DEBUG, " %.3f  ", observation_sums[i]);
    }
    LOG_HMM_BARE(HMM_DEBUG, "\n");
#endif
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

#ifdef _USE_UPDATED_
void BaumWelchGammaSolveHMM( hidden_markov_model_t * model )
{
    /* Update state expectation matrix */
    uint8_t k = model->O.prev, l = model->O.curr;
    LOG_HMM(HMM_DEBUG, "%4d: %s,%s  ", ++count, observation_strings[k], observation_strings[l] );
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            model->Es[k][l][i][j] = ForwardBackward(model, k, l, i, j);
            LOG_HMM_BARE(HMM_DEBUG, "%.4f%s", model->Es[k][l][i][j], i&&j?" ":",");
        }
    }
    /* Update maximum expectation matarix */
    bool diff = ( k != l );
    double observation_max = 0.;
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        double curr_max = 0., prev_max = 0., curr = 0., diag = ForwardBackward( model, k, l, i, i );
        if( !diff )
            curr_max = diag;
        else
        {
            // Assume l comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = ForwardBackward( model, k, l, j, i );// model->Es[k][l][j][i];
                if( curr > curr_max )
                    curr_max = curr;
            }
            
            // Assume k comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = ForwardBackward( model, k, l, i, j );// model->Es[k][l][i][j];
                if( curr > prev_max )
                    prev_max = curr;
            }
        }
        if( diff )
        {
            model->G.value[k][i] = prev_max;
            model->G.cumulative_value[k][i] += prev_max;
        }
        model->G.value[l][i] = curr_max;
        model->G.cumulative_value[l][i] += curr_max;
        
        SET_MAX(observation_max, curr_max);
    }
    
    /* Update gamma expectation maximums */
    if( diff )
        model->G.maximum[k] += observation_max;
    model->G.maximum[l] += observation_max;
    
    LOG_HMM_BARE(HMM_DEBUG, "\n");
    
    UpdateObservationMatrixHMM(model);
}
#else
void BaumWelchGammaSolveHMM( hidden_markov_model_t * model )
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
    /* Update maximum expectation matarix */
    bool diff = ( k != l );
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        double curr_max = 0., prev_max = 0., curr = 0.;
        
        if( !diff )
        {
            curr_max = model->Es[k][l][i][i];
        }
        else
        {
            // assume l comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = model->Es[k][l][j][i];
                if( curr > curr_max )
                {
                    curr_max = curr;
                }
            }
            
            // assume k comes from i
            for( uint8_t j = 0; j < NUM_STATES; j++ )
            {
                curr = model->Es[k][l][i][j];
                if( curr > prev_max )
                {
                    prev_max = curr;
                }
            }
        }
        model->G[k][i] = prev_max;
        model->G[l][i] = curr_max;
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
#endif
