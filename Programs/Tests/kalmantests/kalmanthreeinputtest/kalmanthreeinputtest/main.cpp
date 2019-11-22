//
//  main.cpp
//  kalmanthreeinputtest
//
//  Created by Matthew Fonken on 11/15/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <random>
#include <unistd.h>

#include "kalman.h"
#include "matvec.h"
#include "sweeper.hpp"

#define DEFAULT_LIFESPAN 1.0 // sec
#define DEFAULT_MIN_VALUE 0.
#define DEFAULT_MAX_VALUE 100.
#define DEFAULT_VU      0.1       // Value uncertainty
#define DEFAULT_BU      0.001         // Bias uncertainty
#define DEFAULT_SU      0.001        // Sensor uncertainty
#define DEFAULT_KALMAN_UNCERTAINTY \
(kalman_uncertainty_c){  DEFAULT_VU, DEFAULT_BU, DEFAULT_SU }

#define DEFAULT_VU_2    0.001       // Value uncertainty
#define DEFAULT_BU_2    0.001         // Bias uncertainty
#define DEFAULT_SU_2    0.1        // Sensor uncertainty
#define DEFAULT_KALMAN_UNCERTAINTY_2 \
(kalman_uncertainty_c){  DEFAULT_VU_2, DEFAULT_BU_2, DEFAULT_SU_2 }

#define DEFAULT_SWEEP_STEPS   100
#define DEFAULT_SWEEP_START   0
#define DEFAULT_SWEEP_STOP    500

#define CYCLES 1000

void printGaussian1D( const char * s, gaussian1d_t &g )
{
    printf("%s: %6.3f | %6.3f\n", s, g.mean, g.std_dev );
}

void printGaussian2D( const char * s, gaussian2d_t &g )
{
    printf("%s: (%6.3f, %6.3f) [%6.3f, %6.3f, %6.3f, %6.3f]\n", s, g.mean.a, g.mean.b, g.covariance.a, g.covariance.b, g.covariance.c, g.covariance.d );
}

int main(int argc, const char * argv[]) {

    kalman_filter_t k1, k2;
    
    Sweeper sw1("sw1", DEFAULT_SWEEP_STEPS, DEFAULT_SWEEP_START, DEFAULT_SWEEP_STOP);
    Sweeper sw2("sw2", DEFAULT_SWEEP_STEPS, DEFAULT_SWEEP_START, DEFAULT_SWEEP_STOP);
    Sweeper sw3("sw3", DEFAULT_SWEEP_STEPS, DEFAULT_SWEEP_START, DEFAULT_SWEEP_STOP);
    
    Kalman.Initialize( &k1, DEFAULT_SWEEP_START, DEFAULT_LIFESPAN, DEFAULT_MIN_VALUE, DEFAULT_MAX_VALUE, DEFAULT_KALMAN_UNCERTAINTY );
    Kalman.Initialize( &k2, DEFAULT_SWEEP_START, DEFAULT_LIFESPAN, DEFAULT_MIN_VALUE, DEFAULT_MAX_VALUE, DEFAULT_KALMAN_UNCERTAINTY );
    
    for(int ii = 0; ii < CYCLES; ii++ )
    {
        floating_t r1 = rand() % 100 / 50. - 1.;
//        floating_t r2 = rand() % 100 / 50. - 1.;
        printf("Random number: %f\n", r1);
        double v = sw1.trigger();
//        Kalman.Step( &k1, v + r1 * 50, sw1.step * sw1.dir * r1 * 3);
        Kalman.Step( &k1, v, sw1.step );// * sw2.dir * r2 );
        Kalman.Step( &k2, v + r1 * 50, sw1.step * sw1.dir * r1 );
//        Kalman.Print( &k1 );
//        Kalman.Print( &k2 );
        
//        gaussian2d_t g1 = { { k1.value, k1.velocity }, *(mat2x2*)k1.P };
//        gaussian2d_t g2 = { { k2.value, k2.velocity }, *(mat2x2*)k2.P };
//        gaussian2d_t gc;
//
//        MatVec.Gaussian2D.Multiply( &g1, &g2, &gc );
//        printGaussian2D( "G1", g1 );
//        printGaussian2D( "G2", g2 );
//        printGaussian2D( "GC", gc );
        
        gaussian1d_t g1 = { k1.value, fabs(k1.K[1]) };
        gaussian1d_t g2 = { k2.value, fabs(k2.K[1]) };
        gaussian1d_t gc;
        
        MatVec.Gaussian1D.Multiply( &g1, &g2, &gc );
        printGaussian1D( "G1", g1 );
        printGaussian1D( "G2", g2 );
        printGaussian1D( "GC", gc );
        
        usleep(250000);
    }
    return 0;
}
