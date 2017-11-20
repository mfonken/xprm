#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_LEN 100
#define SCALE   10

typedef double FLOAT;

typedef struct
{
    FLOAT   *array;
    uint16_t length;
} gaussian_t;

void initGaussian( gaussian_t * g, int l )
{
    (*g).array = (FLOAT*)malloc(sizeof(FLOAT)*l);
    (*g).length = 0;
}

FLOAT gaussianDistribution( int x, int mu, FLOAT sigma)
{
    int d = x - mu;
    FLOAT n = 1.0 / (sqrt(2 * M_PI) * sigma);
    return exp(-d*d/(2 * sigma * sigma)) * n;
};

void generateGaussian( gaussian_t * g, int l, FLOAT s )
{
    int m = floor(l/2);
    for(int i = 0, d = l; i <= m; i++, d--)
    {
        g->array[i] = gaussianDistribution(i,m,s);
        if(i != d) g->array[d] = g->array[i];
    }
    g->length = l;
}

void gaussianConvolve( float * arr, size_t len, gaussian_t * g )
{
    
    size_t n, k_len = g->length;
    
    double res[len + k_len - 1];
    
    for (n = 0; n < len + k_len - 1; n++)
    {
        size_t kmin, kmax, k;
        
        res[n] = 0;
        
        kmin = (n >= k_len - 1) ? n - (k_len - 1) : 0;
        kmax = (n < len - 1) ? n : len - 1;
        
        for (k = kmin; k <= kmax; k++)
        {
            res[n] += (FLOAT)arr[k] * g->array[n - k];
        }
    }
    int offset = (int)(( k_len - 1 ) / 2);
    for (n = 0; n < len; n++)
    {
        arr[n] = res[n + offset];
    }
}

void gaussianCrossCorrelate( float * arr, size_t len, gaussian_t * g )
{
    size_t n, k_len = g->length;
    
    double res[len + k_len - 1];
    
    for (n = 0; n < len + k_len - 1; n++)
    {
        size_t kmin, kmax, k;
        
        res[n] = 0;
        
        kmin = (n >= k_len - 1) ? n - (k_len - 1) : 0;
        kmax = (n < len - 1) ? n : len - 1;
        
        for (k = kmin; k <= kmax; k++)
        {
            res[n] += (FLOAT)arr[k] * g->array[n - k];
        }
    }
    int offset = (int)(( k_len - 1 ) / 2);
    for (n = 0; n < len; n++)
    {
        arr[n] = res[n + offset];
    }
}

void displayArray( char[] name, double * arr, int len )
{
    Mat M(SCALE*SCALE, len, CV_8UC3, Scalar(0,0,0));
    Vec3b red   = {   0,   0, 255};
    for(int i = 0; i < len; i++) M.at<Vec3b>(x,pos) = red;
    
    imshow(name, M);
}

int main( int argc, char * args[] )
{
    int l = 11;
    FLOAT s = 1.0;
    if(argc >= 2) l = atoi(args[1]);
    if(argc >= 3) s = atof(args[2]);
    
    printf("Generating gaussian of length %d and sigma %.2f:\n", l, s);
    gaussian_t g;
    initGaussian(&g, MAX_LEN);
    generateGaussian(&g, l, s);
    printGaussian(&g);
    
    return 0;
}
