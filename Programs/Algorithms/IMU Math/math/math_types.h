/***********************************************************************************************//**
 * \file   kinetic_types.h
 * \brief  Types for kinetic math and filtering
 ***************************************************************************************************
 *      Author: Matthew Fonken
 **************************************************************************************************/

#ifndef KINETIC_KINETIC_TYPES_H_
#define KINETIC_KINETIC_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include "kalman.h"
#include "quaternion.h"
#include "matrix.h"
    
/***********************************************************************************************//**
 * @addtogroup Application
 * @{
 **************************************************************************************************/

/***********************************************************************************************//**
 * @addtogroup kinetic
 * @{
 **************************************************************************************************/
 
/***************************************************************************************************
 Public MACROS
 ***************************************************************************************************/
    
#define RAD_TO_DEG (180 / M_PI)
#define DEG_TO_RAD (M_PI / 180)
    
#define ZDIV_LNUM 1 << 10
#define ZDIV(X,Y) Y==0?ZDIV_LNUM:X/Y
#define SIGN(X)   X>=0?1:-1
#define RASIN(X)  X>-1&&X<1?asin(X):SIGN(X)*M_PI/2
    
/* Kalman Defaults */
#define MOTION_MAX_KALMAN_LIFE     10.0
#define MOTION_VALUE_UNCERTAINTY   0.01
#define MOTION_BIAS_UNCERTAINTY    0.003
#define MOTION_SENSOR_UNCERTAINTY  0.02
    
/***************************************************************************************************
 Public Types
 ***************************************************************************************************/
    
typedef double mfloat;
    
static mfloat seconds_since( double time )
{
    struct timeval now;
    gettimeofday( &now, NULL );
    return (now.tv_sec + now.tv_usec/1000000.0) - time;
}
    
/** Kalman structure */
//typedef struct _kalman_t
//{
//    mfloat      K[2];               /**< Gain matrix */
//    mfloat      P_k[2][2];          /**< Error covariance matrix */
//    mfloat      rate;               /**< Rate */
//    mfloat      bias;               /**< Bias */
//    mfloat      value;              /**< Value */
//    uint32_t    timestamp;          /**< Timestamp */
//} kalman_t;
    
/** Kinetic Type */
typedef struct _kinetic_t
{
    mfloat     nongrav[3];
    
    mfloat     position[3];             /**< Raw position */
    mfloat     rotation[3];             /**< Raw rotation */
    mfloat     truePosition[3];         /**< Raw true location */
    
    kalman_t   positionFilter[3];       /**< Filtered position */
    kalman_t   rotationFilter[3];       /**< Filtered rotation */
    kalman_t   truePositionFilter[3];   /**< Filtered true location */
} kinetic_t;

/** 2D Cartesian Cooridinate */
typedef struct _cartesian2_t
{
    mfloat x;                   /**< X value */
    mfloat y;                   /**< Y value */
} cartesian2_t;

/** Tait-Bryan Angles */
typedef struct _ang3_t
{
    mfloat    x;                /**< Phi value */
    mfloat    y;                /**< Theta value */
    mfloat    z;                /**< Psi value */
} ang3_t;

/*! Simple 3d Vector */
typedef struct _vec3_t
{
    mfloat    i;             /**< \$f\hat{i}\$f value */
    mfloat    j;             /**< \$f\hat{j}\$f value */
    mfloat    k;             /**< \$f\hat{k}\$f value */
} vec3_t;
    
/*! Simple 3d Vector */
typedef struct _mat3x3_t
{
    mfloat    ii;               /**< \$f\hat{ii}\$f value */
    mfloat    ji;               /**< \$f\hat{ji}\$f value */
    mfloat    ki;               /**< \$f\hat{ki}\$f value */
    mfloat    ij;               /**< \$f\hat{ij}\$f value */
    mfloat    jj;               /**< \$f\hat{jj}\$f value */
    mfloat    kj;               /**< \$f\hat{kj}\$f value */
    mfloat    ik;               /**< \$f\hat{ik}\$f value */
    mfloat    jk;               /**< \$f\hat{jk}\$f value */
    mfloat    kk;               /**< \$f\hat{kk}\$f value */
} mat3x3_t;

/*! Simple 2d Vector */
typedef struct _vec2_t
{
    mfloat    i;             /**< \$f\hat{i}\$f value */
    mfloat    j;             /**< \$f\hat{j}\$f value */
} vec2_t;
    
//typedef struct
//{
//    double 	accel[3];
//    double 	gyro[3];
//    double 	mag[3];
//    
//    double  accel_res;
//    double 	gyro_res;
//    double 	mag_res;
//    
//    double 	accel_bias[3];
//    double 	gyro_bias[3];
//    double 	mag_bias[3];
//    
//    double	roll;
//    double	pitch;
//    double	yaw;
//} imu_t;
//
//typedef struct
//{
//    imu_t               data;
//    LSM9DS1_cfg_t       settings;
//} LSM9DS1_t;

/** @} (end addtogroup kinetic) */
/** @} (end addtogroup Application) */

#ifdef __cplusplus
}
#endif
    
#endif /* KINETIC_KINETIC_TYPES_H_ */
