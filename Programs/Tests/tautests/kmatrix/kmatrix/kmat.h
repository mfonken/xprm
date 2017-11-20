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
    
#include "global_types.h"
#include "kalman.h"
    
#define NO_PAIR 0x00
    
void printKM( kmat_t * m );
void initKmatPair( kmat_pair_t * m );
void initKmat( kmat_t * m );
void updateKmatPair( kmat_pair_t * m, peak_list_pair_t * p );
bool isExpired( kalman_t * k );
int updateKmat( kmat_t * m, peak_list_t * p );
bool peakQuickCheck( int v );
double compare( lookup_t * m, kmat_p a, kmat_p b );
//void swap( lookup_t * l, int a, int b );
double weightedValue( kmat_t * m, int i );
void swapKalmanMatrix( kmat_t * m, kmat_p a, kmat_p b );
int partitionKalmanMatrix(kmat_t * m, int l, int h);
void quickSortKalmanMatrix(kmat_t * m, int l, int h);
    
    
    void printNL(int tab);
    void printA(int arr[], int low, int high, int tab);
    void swap(int* a, int* b);
    int partition (int arr[], int low, int high, int tab);
    int quickSort(int arr[], int low, int high, int tab);
    
#ifdef __cplusplus
}
#endif

#endif /* kmat_h */
