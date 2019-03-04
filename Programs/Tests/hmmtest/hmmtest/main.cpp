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
//            hmm.O.data[hmm.O.first] = observations[i];
//            HMMFunctions.BaumWelchSolve( &hmm, 0 );
        }
//        for( uint8_t i = 0; i < NUM_STATES; i++ )
//        {
//            double row_sum = 0.;
//            for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//            {
//                hmm.G[i][j] /= hmm.G[i][NUM_STATES];
//                row_sum += hmm.G[i][j];
//            }
//            for( uint8_t j = 0; j < NUM_OBSERVATION_SYMBOLS; j++ )
//            {
//                hmm.G[i][j] /= row_sum;
//            }
//        }
        for( uint8_t i = 0; i < 10; i++ )
        {
            HMMFunctions.BaumWelchSolve( &hmm, hmm.O.last );
        }
        while(1);
    }
    return 0;
}
