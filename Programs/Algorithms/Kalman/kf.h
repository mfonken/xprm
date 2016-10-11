//
//  kf.h
//  
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
    uint32_t K[2];                  // Kalman gain
    uint32_t P_k[2][2];             // Error covariance matrix
    uint32_t rate;
    uint32_t bias;
    uint32_t value;
};

void kalmanInit(struct kalman *k)
{
    k->P_k[0][0]   = 0;
    k->P_k[0][1]   = 0;
    k->P_k[1][0]   = 0;
    k->P_k[1][1]   = 0;
    k->K[0]        = 0;
    k->K[1]        = 0;
    k->rate        = 0;
    k->bias        = 0;
    k->value       = 0;
}

void kalmanUpdate(struct kalman *k,
                  uint32_t value_new,
                  uint32_t rate_new,
                  uint32_t delta_time)
{
    k->rate       = rate_new - k->bias;
    k->value     += delta_time * k->rate;
    uint32_t P_k_diag = delta_time * k->P_k[1][1];
    
    k->P_k[0][0] += delta_time *
                    ( delta_time * k->P_k[1][1] -
                      k->P_k[0][1] -
                      k->P_k[1][0] +
                      VALUE_UNCERTAINTY );
    k->P_k[0][1] -= P_k_diag;
    k->P_k[1][0] -= P_k_diag;
    k->P_k[1][1] += BIAS_UNCERTAINTY * delta_time;
    float S       = k->P_k[0][0] + SENSOR_UNCERTAINTY;
    k->K[0]       = k->P_k[0][0] / S;
    k->K[1]       = k->P_k[1][0] / S;
    uint32_t delta_value = value_new - k->value;
    k->value     += k->K[0] * delta_value;
    k->bias      += k->K[1] * delta_value;
    k->P_k[0][0] -= k->K[0] * k->P_k[0][0];
    k->P_k[0][1] -= k->K[0] * k->P_k[0][1];
    k->P_k[1][0] -= k->K[1] * k->P_k[0][0];
    k->P_k[1][1] -= k->K[1] * k->P_k[0][1];
};

#endif /* kf_h */
