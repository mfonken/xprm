//
//  kf.c
//  
//
//  Created by Matthew Fonken on 10/6/16.
//
//

#include "kf.h"

int main()
{
    struct kalman k;
    kalmanInit(&k);
    kalmanUpdate(&k, 0, 0, 0);
    
    printf("%i\n", k.value);
    return 0;
}
