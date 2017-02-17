/***********************************************************************************************//**
 * \file   kinetic_types.h
 * \brief  Types for kinetic math and filtering
 ***************************************************************************************************
 *      Author: Matthew Fonken
 **************************************************************************************************/

#ifndef KINETIC_KINETIC_TYPES_H_
#define KINETIC_KINETIC_TYPES_H_
    
/** Kalman structure */
typedef struct _kalman_t
{
    double      K[2];               /**< Gain matrix */
    double      P_k[2][2];          /**< Error covariance matrix */
    double      rate;               /**< Rate */
    double      bias;               /**< Bias */
    double      value;              /**< Value */
    uint32_t    timestamp;          /**< Timestamp */
} kalman_t;
    
/** Kinetic Type */
typedef struct _kinetic_t
{
    double     position[3];             /**< Raw position */
    double     rotation[3];             /**< Raw rotation */
    double     truePosition[3];         /**< Raw true location */
    
    kalman_t   positionFilter[3];       /**< Filtered position */
    kalman_t   rotationFilter[3];       /**< Filtered rotation */
    kalman_t   truePositionFilter[3];   /**< Filtered true location */
} kinetic_t;

/** 2D Cartesian Cooridinate */
typedef struct _cartesian2_t
{
    double x;                   /**< X value */
    double y;                   /**< Y value */
} cartesian2_t;

/** Tait-Bryan Angles */
typedef struct _ang3_t
{
    double    a;                /**< Phi value */
    double    b;                /**< Theta value */
    double    c;                /**< Psi value */
} ang3_t;

/*! Simple 3d Vector */
typedef struct _vec3_t
{
    double    i;             /**< \$f\hat{i}\$f value */
    double    j;             /**< \$f\hat{j}\$f value */
    double    k;             /**< \$f\hat{k}\$f value */
} vec3_t;

/*! Simple 2d Vector */
typedef struct _vec2_t
{
    double    i;             /**< \$f\hat{i}\$f value */
    double    j;             /**< \$f\hat{j}\$f value */
} vec2_t;

/*! Quaternion hypercomplex type */
typedef struct
{
    double x;
    double y;
    double z;
    double w;
} quaternion_t;


#endif /* KINETIC_KINETIC_TYPES_H_ */
