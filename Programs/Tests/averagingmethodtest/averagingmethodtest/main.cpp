#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <sys/time.h>

//#define TIME_EACH

#define L 500000
#define MAX 60000

typedef float floating_t;

static inline double timeDiff( struct timeval a, struct timeval b )
{ return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0) + 0.0005; }

template <typename T>
void cma( T v, T i, T *m0, T *m1, T *n )
{
    floating_t n_=1/(++(*n));
    *m0+=((v-*m0)*n_);
    *m1+=(((v*i)-*m1)*n_);
}
template <typename T>
void sma( T v, T i, T *m0, T *m1, T *n )
{
    ++(*n);
    *m0+=v;
    *m1+=v*i;
}

int main(int argc, char * argv[])
{
    struct timeval a,b;
    double t[L] = { 0. };
    double tc_m0 = 0, tc_m1 = 0, tc_count = 0;
    double ts_m0 = 0, ts_m1 = 0, ts_count = 0;
    
    uint16_t arr[L] = { 0 };
    for( uint32_t i = 0; i < L; i++ )
        arr[i] = uint16_t( rand() % MAX );
    
    floating_t cma_m0 = 0, cma_m1 = 0, cma_count = 0;
#ifdef TIME_EACH
    for( uint32_t i = 0; i < L; i++ )
    {
        gettimeofday( &a, NULL);
        cma<floating_t>( (floating_t)arr[i], (floating_t)i, &cma_m0, &cma_m1, &cma_count );
        gettimeofday( &b, NULL);
        t[i] = timeDiff(a,b);
        cma<double>( t[i], (double)i, &tc_m0, &tc_m1, &tc_count );
    }
    
    floating_t sma_m0 = 0, sma_m1 = 0, sma_count = 0;
    for( uint32_t i = 0; i < L; i++ )
    {
        gettimeofday( &a, NULL);
        sma<floating_t>( (floating_t)arr[i], (floating_t)i, &sma_m0, &sma_m1, &sma_count );
        gettimeofday( &b, NULL);
        t[i] = timeDiff(a,b);
        cma<double>( t[i], (double)i, &ts_m0, &ts_m1, &ts_count );
    }
#else
    int i = 0;
    gettimeofday( &a, NULL);
    for( uint32_t i = 0; i < L; i++ )
    {
        cma<floating_t>( (floating_t)arr[i], (floating_t)i, &cma_m0, &cma_m1, &cma_count );
    }
    gettimeofday( &b, NULL);
    t[i] = timeDiff(a,b);
    cma<double>( t[i], (double)i, &tc_m0, &tc_m1, &tc_count );
    
    floating_t sma_m0 = 0, sma_m1 = 0, sma_count = 0;
    
    gettimeofday( &a, NULL);
    for( uint32_t i = 0; i < L; i++ )
    {
        sma<floating_t>( (floating_t)arr[i], (floating_t)i, &sma_m0, &sma_m1, &sma_count );
    }
    gettimeofday( &b, NULL);
    t[i] = timeDiff(a,b);
    cma<double>( t[i], (double)i, &ts_m0, &ts_m1, &ts_count );
#endif
    
    sma_m0 /= sma_count;
    sma_m1 /= sma_count;
    
    printf("cma: count=%d | m0=%3.5f | m1=%3.5f\n", (uint32_t)cma_count, cma_m0, cma_m1 );
    printf("sma: count=%d | m0=%3.5f | m1=%3.5f\n", (uint32_t)sma_count, sma_m0, sma_m1 );
    
    floating_t m0_error = fabs( sma_m0 - cma_m0 ) / sma_m0 * 100.,
    m1_error = fabs( sma_m1 - cma_m1 ) / sma_m1 * 100.,
    m1_m0_cma = cma_m1 / cma_m0,
    m1_m0_sma = sma_m1 / sma_m0,
    m1_m0_error = fabs( m1_m0_sma - m1_m0_cma ) / m1_m0_sma * 100.;
    
    printf("error: m0=%3.10f%% | m1=%3.10f%% | m1/m0=%3.10f%%\n", m0_error, m1_error, m1_m0_error );
    
    printf("time: cma=%3.5fs | sma=%3.5fs\n", tc_m0, ts_m0);
    return 0;
}
