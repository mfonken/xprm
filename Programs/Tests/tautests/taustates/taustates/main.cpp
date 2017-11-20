//
//  main.cpp
//  taustates
//
//  Created by Matthew Fonken on 10/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>
#include "statem.h"

#define MAX_LOOPS 100

using namespace std;

int main(int argc, const char * argv[])
{
    printf("Starting...\n");
    
    system_t sys;
    initSystem(&sys);

    prediction_pair_t pred_pair;
    pred_pair.selection_pair = SIMILAR;
    pred_pair.x.primary_probability = 0.8;
    pred_pair.x.secondary_probability = 0.2;
    pred_pair.y.primary_probability = 0.75;
    pred_pair.y.secondary_probability = 0.1;
    
    for( int i = 0; i < MAX_LOOPS; i++ )
    {
        printf("Interation #%d:\n", i);
        updateSystem( &sys, &pred_pair );
        printBayesianMap(&sys.probabilities);
        updateState( &sys );
        
        if( sys.state == STABLE_SINGLE )
        {
            pred_pair.x.primary_probability     = 0.8;
            pred_pair.x.secondary_probability   = 0.75;
            pred_pair.y.primary_probability     = 0.75;
            pred_pair.y.secondary_probability   = 0.8;
            pred_pair.x.alternate_probability   = 0.05;
            pred_pair.y.alternate_probability   = 0.4;
        }
        else if( sys.state == STABLE_DOUBLE )
        {
            pred_pair.x.primary_probability     = 0.8;
            pred_pair.x.secondary_probability   = 0.64;
            pred_pair.y.primary_probability     = 0.75;
            pred_pair.y.secondary_probability   = 0.8;
            pred_pair.x.primary_probability     = 0.8;
            pred_pair.x.alternate_probability   = 0.9;
            pred_pair.y.alternate_probability   = 0.75;
        }
        else if( sys.state == STABLE_MANY )
        {
            pred_pair.x.primary_probability     = 0.8;
            pred_pair.x.secondary_probability   = 0.1;
            pred_pair.y.primary_probability     = 0.75;
            pred_pair.y.secondary_probability   = 0.2;
            pred_pair.x.alternate_probability   = 0.2;
            pred_pair.y.alternate_probability   = 0.3;
        }
        
        cin.get();
    }
    
    return 0;
}
