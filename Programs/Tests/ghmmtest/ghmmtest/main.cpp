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

transition_matrix_t A =
{
    { 0.5, 0.25, 0.05, 0.2 },
    { 0.2, 0.5, 0.25, 0.05 },
    { 0.05, 0.2, 0.5, 0.25 },
    { 0.25, 0.05, 0.2, 0.5 }
};
observation_matrix_t B =
{
    { 0.,  5. },
    { 10., 5. },
    { 20., 5. },
    { 30., 5. }
};
state_vector_t pi =
{
    0.1,
    0.3,
    0.5,
    0.1,
};

psm_t psm;
Set set;
gaussian1d_t a, b;
int n;

int main(int argc, const char * argv[])
{
    PSMFunctions.Initialize( &psm, &A, &B, &pi );
    HMMFunctions.Print( &psm.hmm );
    
    n = 4;
    double stdv = 10.;
    
    set.Generate( n, (gaussian1d_t){ 10., stdv } );
    set.Generate( n, (gaussian1d_t){ 20., stdv } );
    set.Generate( n, (gaussian1d_t){ 0., stdv } );
    set.Generate( n, (gaussian1d_t){ 30., stdv } );
    
    for( int i = 0; i < set.Length(); i++ )
    {
        auto c = set.Pull();
        printf("Reporting(%2d): %.4f\n", i, c.first);
        HMMFunctions.ReportObservation( &psm.hmm, (hmm_observation_t){ c.first } );
    }
    HMMFunctions.BaumWelchSolve( &psm.hmm, HMM_DELTA );
    HMMFunctions.Print( &psm.hmm );
    
    return 0;
}
