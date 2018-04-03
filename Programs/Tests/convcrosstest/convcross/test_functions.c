#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "test_functions.h"

void initGaussian( gaussian_t * g, INT l )
{
    (*g).array = (FLOAT*)malloc(sizeof(FLOAT)*l);
    (*g).length = 0;
}

FLOAT gaussianDistribution( INT x, INT mu, FLOAT sigma)
{
    INT d = x - mu;
    FLOAT n = 1.0 / (sqrt(2 * M_PI) * sigma);
    return exp(-d*d/(2 * sigma * sigma)) * n;
};

void generateGaussian( gaussian_t * g, INT l, FLOAT s )
{
    if( g->length == l && g->sigma == s ) return;
    INT m = floor(l*SKEW);
    for(INT i = 0, d = l; i <= m; i++, d--)
    {
        g->array[i] = gaussianDistribution(i,m,s);
        if(i != d) g->array[d] = g->array[i];
    }
    g->length = l;
    g->sigma = s;
}

void convolve( FLOAT * arr1, INT len1, FLOAT * arr2, INT len2 )
{
    size_t n;
    
    FLOAT res[len1 + len2 - 1];
    
    for (n = 0; n < len1 + len2 - 1; n++)
    {
        size_t kmin, kmax, k;
        
        res[n] = 0;
        
        kmin = (n >= len2 - 1) ? n - (len2 - 1) : 0;
        kmax = (n < len1 - 1) ? n : len1 - 1;
        
        for (k = kmin; k <= kmax; k++)
        {
            res[n] += arr1[k] * arr2[n - k];
        }
    }
    INT offset = (INT)(( len2 - 1 ) / 2);
    for (n = 0; n < len1; n++)
    {
        arr1[n] = res[n + offset];
    }
}

void crossCorrelate( FLOAT * arr1, INT len1, FLOAT * arr2, INT len2 )
{
    INT n;
    
    FLOAT res[len1];
    
    for (n = 0; n < len1; n++)
    {
        INT kmax, k;
        
        res[n] = 0;
        
        kmax = ( len2 < len1 - 1 - n ) ? len2:len1 - 1 - n;
        
        for (k = 0; k <= kmax; k++)
        {
            res[n] += arr2[k] * arr1[n + k];
        }
    }
    
    for (n = len2/2; n < len1; n++)
    {
        arr1[n] = res[n - len2/2];
    }
}

void gaussianCrossCorrelateOld( FLOAT * arr, INT len, gaussian_t * g )
{
    INT n, len2 = g->length;
    
    FLOAT res[len + len2 - 1];
    
    for (n = 0; n < len; n++)
    {
        INT kmax, k;
        
        res[n] = 0;
        
        kmax = (len2 < len - 1 - n) ? len2 : len - 1 - n;
        
        for (k = 0; k <= kmax; k++)
        {
            res[n] += g->array[k] * arr[n + k];
        }
    }
    
    INT offset = (n-(INT)(( len2 - 1 ) / 2) < 0 )? 0:(INT)(( len2 - 1 ) / 2);
    for (n = 0; n < len; n++)
    {
        arr[n] = res[n - offset];
    }
}
