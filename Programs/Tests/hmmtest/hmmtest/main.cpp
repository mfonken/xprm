//
//  main.cpp
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "psm.h"

#define NUM_OBSERVATIONS 3

observation_t observation_list[NUM_OBSERVATIONS] =
{
    { 1, 1, 0 },
    { 1, 1, 1 },
    { 2, 3, 2 }
};

double nu = 3;

psm_t psm;

int main(int argc, const char * argv[])
{
    PSMFunctions.Initialize( &psm );
    PSMFunctions.ReportObservations( &psm, observation_list, NUM_OBSERVATIONS );
    PSMFunctions.Update( &psm, nu );
    
    return 0;
}
