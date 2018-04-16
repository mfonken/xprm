//
//  tau.c
//  
//
//  Created by Matthew Fonken on 2/12/17.
//
//

#include "tau.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define xkl 6
#define ykl 9

const double xk[xkl] = { 1, 3, 1, 1, 3, 1};//{ 0, 0, 1, 1, 0, 0, 0 };
const double yk[ykl] = { 1, 3, 1, 0, 0, 0, 1, 3, 1 };//, 1, 0, 1, 1, 0 };

void convolve(const double Signal[], size_t SignalLen,
              const double Kernel[], size_t KernelLen,
              double Result[])
{
    size_t n;
    int i = 0;
    for (n = 0; n < SignalLen + KernelLen - 1; n++)
    {
        size_t kmin, kmax, k;
        
        Result[n] = 0;
        
        kmin = (n >= KernelLen - 1) ? n - (KernelLen - 1) : 0;
        kmax = (n < SignalLen - 1) ? n : SignalLen - 1;
        
        
        for (k = kmin; k <= kmax; k++)
        {
            i++;
            Result[n] += Signal[k] * Kernel[n - k];
        }
    }
    printf("\n%d cycles for convolve\n",i);
}

int main( int argc, char * args[])
{
    FILE *fp;
    FILE *outfile;
    outfile = fopen("output.txt", "w+");
    if(argc >= 2)
        fp=fopen(args[1], "r+");
    else
        fp=fopen("test.txt", "r+");
    
    int m = 1, n = 1;
    fscanf(fp, "%i", &m);
    fscanf(fp, "%i", &n);
    
    printf("Scanning in image of size %dx%d.\n", m, n);
    
    int v = 0;
    int image[n][m];
    for(int i = 0; i < n; i++ )
    {
        for(int j = 0; j < m; j++ )
        {
            fscanf(fp, "%i", &v);
            image[i][j] = v;
        }
    }
    for(int i = 0; i < n; i++ )
    {
        for(int j = 0; j < m; j++ )
        {
            printf("%d ", image[i][j]);
        }
        printf("\n");
    }
   
    double x[m], y[n];
    
    v = m;
    double * p;
    p = x;
    while (v-- > 0) *p++ = 0;
    v = n;
    p = y;
    while (v-- > 0) *p++ = 0;
    
    for(int i = 0; i < n; i++ )
    {
        for(int j = 0; j < m; j++ )
        {
            int z = image[i][j];
            x[j] += z;
            y[i] += z;
        }
    }
    
    printf("x:");
    for(int j = 0; j < m; j++ )
    {
        printf("[%.2f]",x[j]);
    }
    printf("\ny:");
    for(int i = 0; i < n; i++ )
    {
        printf("[%.2f]",y[i]);
    }
    printf("\n");
    
    /* x convolution */
    int xcl = m + xkl - 1;
    double xc[xcl];
    convolve(x, m, xk, xkl, xc);
    
    printf("\nxc:");
    for(int i = 0; i < xcl; i++ )
    {
        printf("[%.2f]",xc[i]);
    }
    printf("\n");
    for(int i = 0; i < xcl; i++ )
    {
        printf("%.2f\n",xc[i]);
    }
    
    /* y convolution */
    int ycl = n + ykl - 1;
    double yc[ycl];
    convolve(y, n, yk, ykl, yc);
    
    printf("\nyc:");
    for(int i = 0; i < ycl; i++ )
    {
        printf("[%.2f]",yc[i]);
    }
    printf("\n");
    for(int i = 0; i < ycl; i++ )
    {
        printf("%.2f\n",yc[i]);
    }
    
    int zcl = xcl + ycl - 1;
    double zc[zcl];
    convolve(xc, xcl, yc, ycl, zc);
    printf("\nz:");
    for(int i = 0; i < zcl; i++ )
    {
        printf("[%.2f]",zc[i]);
    }
    printf("\n");
    for(int i = 0; i < zcl; i++ )
    {
        printf("%.2f\n",zc[i]);
    }
    
    fprintf(outfile,"%d %d\n", xcl, ycl);
    for(int i = 0; i < ycl; i++ )
    {
        for(int j = 0; j < xcl; j++ )
        {
            fprintf(outfile, "%.3f ", xc[j] + yc[i]);
        }
        fprintf(outfile, "\n");
    }
    
    
    
    fclose(outfile);
    return 0;
}
