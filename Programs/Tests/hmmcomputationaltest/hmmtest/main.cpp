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
#define DELTA 5.

hidden_markov_model_t hmm;

int main(int argc, const char * argv[])
{
    HMMFunctions.Initialize( &hmm );
    InitTest( &hmm );
    
    while(1)
    {
        int n = sizeof(observations)/sizeof(observations[0]);
        for( int r = 0; r < REPETITIONS; r++ )
        {
            for( uint8_t i = 0; i < n; i++ )
            {
                PushToObservationBuffer(&hmm.O, observations[i]);
            }
            HMMFunctions.BaumWelchSolve( &hmm, DELTA );
        }
        HMMFunctions.Print( &hmm );
         while(1);
    }
    return 0;
}
