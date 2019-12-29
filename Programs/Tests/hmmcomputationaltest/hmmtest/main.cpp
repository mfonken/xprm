//
//  main.cpp
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <string.h>
#include "hmm_test.h"

#include "statistics.h"
#include "timestamp.h"

using namespace std;

#define REPETITIONS 1//00000
#define DELTA 1.

hidden_markov_model_t hmm;

int main(int argc, const char * argv[])
{
    HMMFunctions.Initialize( &hmm, "hmm" );
   
    cumulative_average_t stat = {0};

    InitTest( &hmm );
    HMMFunctions.Print( &hmm );
    int n = sizeof(observations)/sizeof(observations[0]);
    for( int r = 0; r < REPETITIONS; r++ )
    {
        double start_time = TIMESTAMP();
        for( uint8_t i = 0; i < n; i++ )
        {
            PushToObservationBuffer(&hmm.O, observations[i]);
//            HMMFunctions.BaumWelchSolve( &hmm, DELTA );
        }

        HMMFunctions.BaumWelchSolve( &hmm, DELTA );
        CumulateAverageStatistics(SECONDSSINCE(start_time) * 1000000, &stat);
        if(r < REPETITIONS-1) InitTest( &hmm );
    }
    HMMFunctions.Print( &hmm );
    
    printf(">:%.4fµs\n", stat.value);
    return 0;
}
