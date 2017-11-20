//
//  statetest.c
//  tau+
//
//  Created by Matthew Fonken on 10/25/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#define M_HALF_PI M_PI / 2

#define USE_US
//#define TIME_ALL

#ifndef TIME_ALL
#define TIME_IND
#endif

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }// + 0.0005; }

typedef enum { CMATH_SIN = 0, CMATH_COS, QUICK_SIN, QUICK_COS, NUM_METHODS } method;
static inline const char *methodText(method m)
{
    const char *method_strings[] = { "CMATH SIN", "CMATH COS", "QUICK SIN", "QUICK COS" };
    return method_strings[m];
}

double quickSin( double x )
{
    double sin;
    //always wrap input angle to -PI..PI
    if (x < -3.14159265)
        x += 6.28318531;
    else
        if (x >  3.14159265)
            x -= 6.28318531;
    
    //compute sine
    if (x < 0)
    {
        sin = 1.27323954 * x + 0.405284735 * x * x;
        
        if (sin < 0)
            sin = .225 * (sin *-sin - sin) + sin;
        else
            sin = .225 * (sin * sin - sin) + sin;
    }
    else
    {
        sin = 1.27323954 * x - 0.405284735 * x * x;
        
        if (sin < 0)
            sin = .225 * (sin *-sin - sin) + sin;
        else
            sin = .225 * (sin * sin - sin) + sin;
    }
    return sin;
}

double quickCos( double x )
{
    return quickSin( x + M_HALF_PI );
    double cos;
    //compute cosine: sin(x + PI/2) = cos(x)
    x += 1.57079632;
    if (x >  3.14159265)
        x -= 6.28318531;
    
    if (x < 0)
    {
        cos = 1.27323954 * x + 0.405284735 * x * x;
        
        if (cos < 0)
            cos = .225 * (cos *-cos - cos) + cos;
        else
            cos = .225 * (cos * cos - cos) + cos;
    }
    else
    {
        cos = 1.27323954 * x - 0.405284735 * x * x;
        
        if (cos < 0)
            cos = .225 * (cos *-cos - cos) + cos;
        else
            cos = .225 * (cos * cos - cos) + cos;
    }
    return cos;
     
}

int main( int argc, char * argv[] )
{
    
    int num_iterations =  10;
    if(argc > 1) num_iterations = atoi(argv[1]);
    
    int all = 0;
#ifdef TIME_ALL
    all = 1;
#endif
    
    printf("Starting trig tests with %d iterations. (Timing %s iterations)\n", num_iterations, all?"ALL":"IND");
    
    double values[NUM_METHODS][num_iterations];
    double vdif[NUM_METHODS/2];
    double time[NUM_METHODS][num_iterations];
    double avgs[NUM_METHODS];
    
    double randoms[num_iterations];
    for(int i = 0; i < num_iterations; i++) randoms[i] = ((rand() % 2000) - 1000) * (M_PI*2/1000);
    
    struct timeval start, stop, a, b;
    for(int m = 0; m < NUM_METHODS; m++)
    {
#ifdef TIME_ALL
        gettimeofday(&a, NULL);
#endif
        for(int i = 0; i < num_iterations; i++)
        {
            
            double a = randoms[i], b;
            switch(m)
            {
                case CMATH_SIN:
#ifdef TIME_IND
                    gettimeofday(&start, NULL);
#endif
                    b = cos(a);
#ifdef TIME_IND
                    gettimeofday(&stop, NULL);
#endif
                    break;
                case CMATH_COS:
#ifdef TIME_IND
                    gettimeofday(&start, NULL);
#endif
                    b = cos(a);
#ifdef TIME_IND
                    gettimeofday(&stop, NULL);
#endif
                    break;
                case QUICK_SIN:
#ifdef TIME_IND
                    gettimeofday(&start, NULL);
#endif
                    b = quickSin(a);
#ifdef TIME_IND
                    gettimeofday(&stop, NULL);
#endif
                case QUICK_COS:
#ifdef TIME_IND
                    gettimeofday(&start, NULL);
#endif
                    b = quickCos(a);
#ifdef TIME_IND
                    gettimeofday(&stop, NULL);
#endif
                default:
                    break;
            }
            values[m][i] = b;
            time[m][i] = timeDiff(start, stop);
            
//            printf("%s: random is %f and result is %f\n", methodText(m), a, b);
        }
#ifdef TIME_ALL
        gettimeofday(&b, NULL);
#endif
#ifdef TIME_IND
        avgs[m] = 0;
        for(int i = 0; i < num_iterations; i++)
        {
          avgs[m] += time[m][i];
        }
        avgs[m] /= num_iterations;
#endif
#ifdef TIME_ALL
        avgs[m] = timeDiff(a, b);
#endif
    }
    printf("AVERAGES:\n");
    int mul = 1000000000;
#ifdef USE_US
    mul = 1000000;
#endif
    for(int m = 0; m < NUM_METHODS; m++)
    {
        printf("\t%s: %.6f (%s)\n", methodText(m), avgs[m]*mul, mul==1000000?"µs":"ns");
    }
    
    
    vdif[CMATH_SIN] = 0;
    for(int i = 0; i < num_iterations; i++)
    {
        double a = values[CMATH_SIN][i], b = values[QUICK_SIN][i], c = fabs((a - b));// / b);
        vdif[CMATH_SIN] += c;
//        printf("adding %f\n", c);
    }
    vdif[CMATH_SIN] /= num_iterations;
    printf("AVERAGE SIN DIFF - %f%%\n", vdif[CMATH_SIN] * 100);
    
    vdif[CMATH_COS] = 0;
    for(int i = 0; i < num_iterations; i++)
    {
        double a = values[CMATH_COS][i], b = values[QUICK_COS][i], c = fabs((a - b));// / b);
        vdif[CMATH_COS] += c;
//        printf("adding %f\n", c);
    }
    vdif[CMATH_COS] /= num_iterations;
    printf("AVERAGE COS DIFF - %f%%\n", vdif[CMATH_COS] * 100);
    
    return 0;
}
