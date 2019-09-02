//
//  main.cpp
//  psmtest
//
//  Created by Matthew Fonken on 8/29/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "test_config.h"
#include "psm.h"

int main(int argc, const char * argv[])
{
    psm_t psm;
    PSMFunctions.Initialize( &psm );

    HMMFunctions.Print( &psm.hmm );
    
    return 0;
}
