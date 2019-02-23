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
//        for( uint32_t i = 0; i < NUM_UPDATES; i++ )
//            UpdateFSM( &hmm.A );
        
        HMMFunctions.ForwardSolve( &hmm );
        while(1);
    }
    return 0;
}
