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

/***********************************************************************************************//**
*  \brief Initalize Kalman Filter
*  \param[in] k Pointer to kalman filter type
*  \param[in] v Initial value
**************************************************************************************************/

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
/***********************************************************************************************//**
*  \brief Update Kalman Filter
*  \param[in] k Pointer to kalman filter type
*  \param[in] value_new    Measured value      - units
*  \param[in] rate_new     Measured rate       - units/time
*  \param[in] delta_time   Time since last     - time
***************************************************************************************************
* FORMULA:
\f{eqnarray*}{
\mathbf{Predict\space:} \\
&rate_k &=& rate_{new} - bias_k \\
&value_k &=& value_k + {rate_K}\Delta{t} \\
&P_{k_{diag}} &=& P_{11_k}\Delta{t} \\
&P_{00_k} &=& P_{00_k} + \Delta{t}(P_{11_k}\Delta{t} - P_{01_k} - P_{10_k} + value_u) \\
&P_{01_k} &=& P_{01_k} - P_{k_{diag}} \\
&P_{10_k} &=& P_{10_k} - P_{k_{diag}} \\
&P_{11_k} &=& P_{11_k} + bias_u\Delta{t} \\
\\
\mathbf{Update\space:} \\
&S &=& P_{00_k} + sensor_u \\
&K_{0_k} &=& P_{00_k} / S \\
&K_{1_k} &=& P_{10_k} / S \\
&\Delta{value} &=& value_{new} - value_k \\
&value_k &=& value_k + K_{0_k}\Delta{value} \\
&bias_k &=& bias_k + K_{1_k}\Delta{value} \\
&P_{00_k} &=& P_{00_k} - K_{0_k}P_{00_k} \\
&P_{01_k} &=& P_{01_k} - K_{0_k}P_{01_k} \\
&P_{10_k} &=& P_{10_k} - K_{1_k}P_{00_k} \\
&P_{11_k} &=& P_{11_k} - K_{1_k}P_{01_k} \\
\f}
**************************************************************************************************/
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
