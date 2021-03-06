//
//  wiki_bw_example.h
//  psmtest
//
//  Created by Matthew Fonken on 9/1/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef wiki_bw_example_h
#define wiki_bw_example_h

//
//  main.cpp
//  psmtest
//
//  Created by Matthew Fonken on 8/29/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "test_config.h"
#include "psm.h"

static observation_symbol_t O[]
{
    ZERO_SYMBOL,
    ZERO_SYMBOL,
    ZERO_SYMBOL,
    ZERO_SYMBOL,
    ZERO_SYMBOL,
    MANY_SYMBOL,
    MANY_SYMBOL,
    ZERO_SYMBOL,
    ZERO_SYMBOL,
    ZERO_SYMBOL,
};

int main(int argc, const char * argv[])
{
    psm_t psm;
    PSMFunctions.Initialize( &psm );
    psm.hmm.A[0][0] = 0.5;
    psm.hmm.A[0][1] = 0.5;
    psm.hmm.A[1][0] = 0.3;
    psm.hmm.A[1][1] = 0.7;
    psm.hmm.B[0][0] = 0.3;
    psm.hmm.B[1][0] = 0.7;
    psm.hmm.B[0][1] = 0.8;
    psm.hmm.B[1][1] = 0.2;
    psm.hmm.pi[0] = 0.2;
    psm.hmm.pi[1] = 0.8;
    
    int N = sizeof(O)/sizeof(O[0]);
    for( int i = 0; i < N; i++ )
    {
        HMMFunctions.ReportObservation( &psm.hmm, O[i] );
    }
    HMMFunctions.BaumWelchSolve( &psm.hmm, HMM_UPDATE_DELTA );
    
    return 0;
}


#endif /* wiki_bw_example_h */
