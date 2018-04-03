//
//  test_functions.h
//  convcross
//
//  Created by Matthew Fonken on 10/5/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef test_functions_h
#define test_functions_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "global_types.h"

#define MAX_LEN 1000
#define SCALE   100
#define SKEW    0

void initGaussian( gaussian_t * g, INT l );
FLOAT gaussianDistribution( INT x, INT mu, FLOAT sigma);
void generateGaussian( gaussian_t * g, INT l, FLOAT s );
void convolve( FLOAT * arr1, INT len1, FLOAT * arr2, INT len2 );
void crossCorrelate( FLOAT * arr1, INT len1, FLOAT * arr2, INT len2 );

#ifdef __cplusplus
    }
#endif
    
#endif /* test_functions_h */
