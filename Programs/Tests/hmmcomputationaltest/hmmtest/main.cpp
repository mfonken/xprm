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

#define NUM_UPDATES 30

hidden_markov_model_t hmm;

int main(int argc, const char * argv[])
{
    HMMFunctions.Initialize( &hmm );
    InitTest( &hmm );
    while(1)
    {
        printf("P>C  S0>S0  S0>S1  S1>S0  S1>S1   S0>N   S1>N   N>MAX  S0>E   S1>E   E>MAX\n");
//        addToObservationBuffer(&hmm.O, observations[0]);
        for( uint8_t i = 0; i < sizeof(observations)/sizeof(observations[0]); i++ )
        {
            reportObservation(&hmm.O, observations[i]);
            HMMFunctions.BaumWelchSolve( &hmm, 0 );
        }
        
        printf("\t ");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                printf("%.4f%s", hmm.Gc[j][i], i&&j?" ":",");
                hmm.Gc[j][i] /= hmm.Gm[j][i];
            }
        }
        printf("\n\t\t %.4f\t\t  %.4f\n", hmm.Gm[0][0],hmm.Gm[1][1]);
        printf("\n");
        printf("|G|:\n");
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
        searchListCombination(0.154);
        while(1);
    }
    return 0;
}
