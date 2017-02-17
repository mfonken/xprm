/***********************************************************************************************//**
 * \file   kalman.h
 * \brief  Quick Kalman Filter Header
 ***************************************************************************************************
 *      Author: Matthew Fonken
 *      Sources:
 **************************************************************************************************/

#ifndef kalman_h
#define kalman_h


/***************************************************************************************************
Public Definitions
***************************************************************************************************/

/** Uncertainty of value */
#define VALUE_UNCERTAINTY   0.001
    
/** Uncertainty of bias */
#define BIAS_UNCERTAINTY    0.003
    
/** Uncertainty of sensor */
#define SENSOR_UNCERTAINTY  0.03
    
/** Kalman structure */
typedef struct
{
    double      K[2];               /**< Gain matrix */
    double      P_k[2][2];          /**< Error covariance matrix */
    double      rate;               /**< Rate */
    double      bias;               /**< Bias */
    double      value;              /**< Value */
    //uint32_t    timestamp;          /**< Timestamp */
} kalman_t;

void Kalman_Init( kalman_t *, double );

void Kalman_Update( kalman_t *, double, double, double );
    
#endif /* kalman_h */
