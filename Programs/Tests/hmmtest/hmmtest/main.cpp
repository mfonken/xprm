//
//  main.cpp
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
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
        addToObservationBuffer(&hmm.O, observations[0]);
        for( uint8_t i = 1; i < 10; i++ )
        {
            addToObservationBuffer(&hmm.O, observations[i]);
            HMMFunctions.BaumWelchSolve( &hmm, 0 );
        }
        printf("\t ");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                printf("%.4f%s", hmm.G.cumulative_value[j][i], i&&j?" ":",");
                hmm.G.cumulative_value[j][i] /= hmm.G.maximum[j];
            }
        }
        printf("\n\t\t %.4f\t\t  %.4f\n", hmm.G.maximum[0], hmm.G.maximum[1]);
        printf("\n");
        printf("|G|:\n");
        for( uint8_t j = 0; j < NUM_STATES; j++ )
        {
            double row_sum = 0.;
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
                row_sum += hmm.G.cumulative_value[i][j];
            }
            for( uint8_t i = 0; i < NUM_OBSERVATION_SYMBOLS; i++ )
            {
//                hmm.G.cumulative_value[i][j] /= row_sum;
                printf("|%.4f|", hmm.G.cumulative_value[i][j]);
                hmm.B.expected[j][i] = hmm.G.cumulative_value[i][j];
            }
            printf("\n");
        }
        while(1);
    }
    return 0;
}
