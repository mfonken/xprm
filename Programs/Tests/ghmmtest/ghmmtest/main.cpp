//
//  main.cpp
//  ghmmtest
//
//  Created by Matthew Fonken on 9/4/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "psm.h"
#include "test_generator.hpp"

#define HMM_DELTA 1.0

psm_t psm;
Set set;
gaussian1d_t A, B;
int n;

int main(int argc, const char * argv[])
{
    PSMFunctions.Initialize( &psm );
    HMMFunctions.Print( &psm.hmm );
    
    A = (gaussian1d_t){ 1., 0.2 };
    B = (gaussian1d_t){ 25, 5 };
    n = 10;
    
    set.Generate( n, A, B );
    for( int i = 0; i < n; i++ )
    {
        auto c = set.Pull();
        HMMFunctions.ReportObservation( &psm.hmm, (hmm_observation_t){ c.first, c.second } );
    }
    HMMFunctions.BaumWelchSolve( &psm.hmm, HMM_DELTA );
    HMMFunctions.Print( &psm.hmm );
    
    return 0;
}
