//
//  main.cpp
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <string.h>
#include "fsm_test.h"

using namespace std;

#define REPETITIONS 1
#define DEFAULT_NU 0.75
#define VARIATION 0.75

hidden_markov_model_t hmm;
kumaraswamy_t kumaraswamy;

int main(int argc, const char * argv[])
{
    HMMFunctions.Initialize( &hmm );
    InitTest( &hmm );
    KumaraswamyFunctions.Initialize( &kumaraswamy, NUM_STATE_GROUPS + 1 );
    
    LOG_HMM(DEBUG_1, "|B0|:\n");
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        double row_s = 0;
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            LOG_HMM_BARE(DEBUG_1, "|%.4f|", hmm.B.expected[j][i]);
            row_s += hmm.B.expected[j][i];
        }
        LOG_HMM_BARE(DEBUG_1, " = %.3f\n", row_s);
    }
    
    double band_list[NUM_STATE_GROUPS] = { 0. };
    
    while(1)
    {
#ifdef SPOOF
        LOG_HMM(DEBUG_1, "P>C  S0>S0  S0>S1  S1>S0  S1>S1   S0>N   S1>N   N>MAX  S0>E   S1>E   E>MAX\n");
#endif
//        addToObservationBuffer(&hmm.O, observations[0]);
        int n = sizeof(observations)/sizeof(observations[0]);
        for( int r = 0; r < REPETITIONS; r++ )
        {
            for( uint8_t i = 0; i < sizeof(observations)/sizeof(observations[0]); i++ )
            {
                double nu = (double)(rand() % 1000) * 2 * VARIATION / 1000 - VARIATION + DEFAULT_NU;
                if(nu < 0) nu = 0;
                int8_t expected_state = UNKNOWN_STATE, nu_symbol = (int8_t)round(nu);
                double max = 0.;
                for( uint8_t i = 0; i < NUM_STATES; i++ )
                {
                    double check = hmm.B.expected[i][nu_symbol];
                    if( check > max )
                    {
                        expected_state = i;
                        max = check;
                    }
                }
                LOG_HMM(DEBUG_2, "Expected state for nu %.4f[%d] is %s\n", nu, nu_symbol, stateString(expected_state));
                
                if(r%2)
                    reportObservation(&hmm.O, observations[i]);
                else
                    reportObservation(&hmm.O, observations[n-1-i]);
                HMMFunctions.BaumWelchGammaSolve( &hmm );
                
                double B_list[NUM_STATES] = { 1 };
                for( uint8_t i = 0; i < NUM_STATES; i++ )
                {
#ifdef SPOOF
                    B_list[i] = hmm.B.expected[i][I];
#else
                    B_list[i] = hmm.B.expected[i][ONE];
#endif
                }
                KumaraswamyFunctions.GetVector( &kumaraswamy, nu, band_list, B_list, NUM_STATE_GROUPS );
                FSMFunctions.Sys.Update( &hmm.A, band_list );
            }
        }
//#ifdef SPOOF
//        LOG_HMM(DEBUG_1, "\t ");
//        for( uint8_t j = 0; j < NUM_STATES; j++ )
//        {
//            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
//            {
//                LOG_HMM_BARE(DEBUG_1, "%.4f%s", hmm.Gc[j][i], i&&j?" ":",");
//                if( hmm.Gm[j][i] != 0)
//                    hmm.Gc[j][i] /= hmm.Gm[j][i];
//            }
//        }
//        LOG_HMM_BARE(DEBUG_1, "\n\t\t %.4f\t\t  %.4f\n", hmm.Gm[0][0],hmm.Gm[1][1]);
//        LOG_HMM(DEBUG_1, "\n");
//#endif
#ifdef _USE_UPDATED_
         LOG_HMM(DEBUG_1, "Gamma:\n");
        /* Observation matrix update */
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            double row_sum = 0.;
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
                row_sum += hmm.B.expected[j][i];
            
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                LOG_HMM_BARE(DEBUG_1, "|%.4f|", hmm.B.expected[j][i]);
            }
            LOG_HMM_BARE(DEBUG_1, " = %.4f\n", row_sum);
        }
#else
        LOG_HMM(DEBUG_1, "|Bf|:\n");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            double row_sum = 0.;
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                row_sum += hmm.Gc[i][j];
            }
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                hmm.Gc[i][j] /= row_sum;
                LOG_HMM_BARE(DEBUG_1, "|%.4f|", hmm.Gc[i][j]);
                hmm.B.expected[j][i] = hmm.Gc[i][j];
            }
            LOG_HMM_BARE(DEBUG_1, "\n");
        }
#endif
//        searchListCombination(0.1344);
        
        LOG(ALWAYS, "Done.\n");
        while(1);
    }
    return 0;
}
