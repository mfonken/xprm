//
//  kmat.h
//  kmatrix
//
//  Created by Matthew Fonken on 10/9/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef kmat_h
#define kmat_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
    
#include "test_setup.h"
    
#include "global_types.h"
#include "kalman.h"
    
void    initKMatPair      ( kmat_pair_t * m                           );
void    getKMatPrediction ( kmat_pair_t * m,    prediction_pair_t * o );
void    updateKMatPair    ( kmat_pair_t * m,    peak_list_pair_t * p  );
void    initKMat          ( kmat_t      * m                           );
int     updateKMat        ( kmat_t      * m,    peak_list_t * p       );
double  weightedValue     ( kmat_t      * m,    int i                 );
bool    isExpired         ( kalman_t    * k                           );
void    swapKMat          ( kmat_t      * m,    kmat_p a,   kmat_p b  );
int     partitionKMat     ( kmat_t      * m,    int l,      int h,    bool w );
void    quickSortKMat     ( kmat_t      * m,    int l,      int h,    bool w );
    
#ifdef __cplusplus
}
#endif

#endif /* kmat_h */
