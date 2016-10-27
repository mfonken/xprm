//
//  kf.h
//
//  Quick Kalman Filter
//
//  Created by Matthew Fonken on 10/6/16.
//
//

#include <stdio.h>
#include <stdint.h>

#ifndef kf_h
#define kf_h

#define VALUE_UNCERTAINTY   0.001
#define BIAS_UNCERTAINTY    0.003
#define SENSOR_UNCERTAINTY  0.03

struct kalman
{
    double K[2];                  // Kalman gain
    double P_k[2][2];             // Error covariance matrix
    double rate;
    double bias;
    double value;
    double timestamp;
};

void initKalman( struct kalman *k,
                 double v )
{
    k->K[0]        = 0;
    k->K[1]        = 0;
    k->P_k[0][0]   = 0;
    k->P_k[0][1]   = 0;
    k->P_k[1][0]   = 0;
    k->P_k[1][1]   = 0;
    k->rate        = 0;
    k->bias        = 0;
    k->value       = v;
}

void updateKalman( struct kalman *k,
                   double value_new,
                   double rate_new,
                   double delta_time )
{
    /* =-----= PREDICT =-----= */
    /* Predict values */
    k->rate       = rate_new - k->bias;
    k->value     += delta_time * k->rate;
    
    /* Predict error covariance */
    double P_k_diag = delta_time * k->P_k[1][1];
    k->P_k[0][0] +=   delta_time *
                    ( delta_time *
                      k->P_k[1][1] -
                      k->P_k[0][1] -
                      k->P_k[1][0] +
                      VALUE_UNCERTAINTY );
    k->P_k[0][1] -= P_k_diag;
    k->P_k[1][0] -= P_k_diag;
    k->P_k[1][1] += BIAS_UNCERTAINTY * delta_time;
    
    /* =-----= UPDATE =-----= */
    /* Update values */
    double S      = k->P_k[0][0] + SENSOR_UNCERTAINTY;
    k->K[0]       = k->P_k[0][0] / S;
    k->K[1]       = k->P_k[1][0] / S;
    double delta_value = value_new - k->value;
    k->value     += k->K[0] * delta_value;
    k->bias      += k->K[1] * delta_value;
    
    /* Update error covariance */
    k->P_k[0][0] -= k->K[0] * k->P_k[0][0];
    k->P_k[0][1] -= k->K[0] * k->P_k[0][1];
    k->P_k[1][0] -= k->K[1] * k->P_k[0][0];
    k->P_k[1][1] -= k->K[1] * k->P_k[0][1];
    
    //timestamp = now();
};

#endif /* kf_h */
