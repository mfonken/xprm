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

#define HMM_DELTA 10.0

void test1( void )
{
    psm_t psm;
    Set set;
    int n;
    
    PSMFunctions.Initialize( &psm );
    HMMFunctions.Print( &psm.hmm );
    
    n = 2;
    double stdv = 10;
    
    for( uint i = 0; i < NUM_STATES; i++ )
        set.Generate( n, (gaussian1d_t){ psm.hmm.B[i].mean, stdv } );
    
    for( int i = 0; i < set.Length(); i++ )
    {
        auto c = set.Pull();
        printf("Reporting(%2d): %.4f\n", i, c.first);
        HMMFunctions.ReportObservation( &psm.hmm, (hmm_observation_t){ c.first } );
    }
    HMMFunctions.BaumWelchSolve( &psm.hmm, HMM_DELTA );
    HMMFunctions.Print( &psm.hmm );
    FSMFunctions.Map.Print( &psm.hmm.A, UNKNOWN_STATE );
}

void test2( void )
{
    psm_t psm;
    Set set;
    int n;
    
    PSMFunctions.Initialize( &psm );
    HMMFunctions.Print( &psm.hmm );
    
    n = 2;
    double stdv = 1;
    
    for( uint i = 0; i < NUM_STATES; i++ )
        set.Generate( n, (gaussian1d_t){ psm.hmm.B[i].mean, stdv } );
    
    for( int i = 0; i < set.Length(); i++ )
    {
        auto c = set.Pull();
        printf("Reporting(%2d): %.4f\n", i, c.first);
        HMMFunctions.ReportObservation( &psm.hmm, (hmm_observation_t){ c.first } );
        HMMFunctions.BaumWelchSolve( &psm.hmm, HMM_DELTA );
    }
    HMMFunctions.Print( &psm.hmm );
    FSMFunctions.Map.Print( &psm.hmm.A, UNKNOWN_STATE );
}

int main(int argc, const char * argv[])
{
    test1();
    for( uint8_t i = 0; i < 100; i++, printf("=")); printf("\n");
    test2();
    return 0;
}
