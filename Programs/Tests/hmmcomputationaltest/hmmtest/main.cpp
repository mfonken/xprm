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

#define REPETITIONS 5000

hidden_markov_model_t hmm;

int main(int argc, const char * argv[])
{
    HMMFunctions.Initialize( &hmm );
    InitTest( &hmm );
    
    printf("|B0|:\n");
    for( uint8_t j = 0; j < NUM_STATES; j++ )
    {
        double row_s = 0;
        for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
        {
            printf("|%.4f|", hmm.B.expected[j][i]);
            row_s += hmm.B.expected[j][i];
        }
        printf(" = %.3f\n", row_s);
    }
    
    while(1)
    {
#ifdef SPOOF
        printf("P>C  S0>S0  S0>S1  S1>S0  S1>S1   S0>N   S1>N   N>MAX  S0>E   S1>E   E>MAX\n");
#endif
//        addToObservationBuffer(&hmm.O, observations[0]);
        int n = sizeof(observations)/sizeof(observations[0]);
        for( int r = 0; r < REPETITIONS; r++ )
        {
            for( uint8_t i = 0; i < sizeof(observations)/sizeof(observations[0]); i++ )
            {
                if(r%2)
                    reportObservation(&hmm.O, observations[i]);
                else
                    reportObservation(&hmm.O, observations[n-1-i]);
                HMMFunctions.BaumWelchTransitionSolve( &hmm );
        }
        }
#ifdef SPOOF
        printf("\t ");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                printf("%.4f%s", hmm.Gc[j][i], i&&j?" ":",");
                if( hmm.Gm[j][i] != 0)
                    hmm.Gc[j][i] /= hmm.Gm[j][i];
            }
        }
        printf("\n\t\t %.4f\t\t  %.4f\n", hmm.Gm[0][0],hmm.Gm[1][1]);
#endif
        printf("\n");
        printf("|Bf|:\n");
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
                printf("|%.4f|", hmm.Gc[i][j]);
                hmm.B.expected[j][i] = hmm.Gc[i][j];
            }
            printf("\n");
        }
//        searchListCombination(0.154);
        while(1);
    }
    return 0;
}
