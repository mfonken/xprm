/*! \file spatial.h
    \brief Spatial Orientation Main\r\n
 
 NOTES:
 - IMU is for rotational data ONLY!
 - Positional data comes from camera triangulation module and beacons
 - Right-handed rotation
 
  Created by Matthew Fonken on 10/8/16.
 */

#ifndef kinetic_h
#define kinetic_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
#include <math.h>

/* Math headers */
#include "../math/imu_math.h"

/* Types */
#include "../filters/kalman.h"

/* Kalman Defaults */
#define MOTION_MAX_KALMAN_LIFE     10.0
#define MOTION_VALUE_UNCERTAINTY   0.01
#define MOTION_BIAS_UNCERTAINTY    0.003
#define MOTION_SENSOR_UNCERTAINTY  0.02
    
/** Absolute value */
#define 	absl(x) x > 0 ? x:-x
    
/** Initial normal unit vector to beacon plane */
#define     VISION_ZSTATE_I  0
#define     VISION_ZSTATE_J  0
#define     VISION_ZSTATE_K -1
    
    /* Camera Precalculated values */
#define     CAMERA_OFFSET_ANGLE_X   0       // degrees
#define     CAMERA_OFFSET_ANGLE_Y   0       // degress
#define     CAMERA_OFFSET_ANGLE_Z   0       // degrees
    
//#define     CAMERA_ALPHA_W          120     // degrees
//#define     CAMERA_ALPHA_H          75      // degrees
    
#define     CAMERA_WIDTH            640//1280    // pixels
#define     CAMERA_HEIGHT           400//800     // pixels
#define     CAMERA_HALF_WIDTH       CAMERA_WIDTH/2
#define     CAMERA_HALF_HEIGHT      CAMERA_HEIGHT/2
    
    
#define     UNITS                  "cm"
#define     PIXEL_TO_UNIT           3e-6
#define     UNIT_TO_PIXEL         ( 1.0 / PIXEL_TO_UNIT )
    
#define     FOCAL_REFRACTION        1.42e-6//1.42e-6//  // units / units
#define     FOCAL_LENGTH          ( 3.5e-3 * FOCAL_REFRACTION ) * UNIT_TO_PIXEL // dimension units
    
#define     D_FIXED                 5.08e-2//3.4e-2
    
    
    double seconds_since( double time )
    {
        struct timeval now;
        gettimeofday( &now, NULL );
        return (now.tv_sec + now.tv_usec/1000000.0) - time;
    }
    
    /** Kinetic Type */
    typedef struct _kinetic_values_t
    {
        double     position[3];             /**< Raw position */
        double     rotation[3];             /**< Raw rotation */
    } kinetic_values_t;
    
    typedef struct _kinetic_filters_t
    {
        kalman_t * position[3];             /**< Raw position */
        kalman_t * rotation[3];             /**< Raw rotation */
    } kinetic_filters_t;
    
    typedef struct _kinetic_t
    {
        kinetic_values_t values;
        kinetic_filters_t filters;
        quaternion_t *
                qd,
                qd_,
                qs,
                qs_,
                qc,
                qc_,
                qr,
                qg,
                qa;
    } kinetic_t;
    
    struct kinetic
    {
        int  (*init)( kinetic_t * k );
        void (*updateRotation)( kinetic_t * k, ang3_t e, ang3_t g);
        void (*updatePosition)( kinetic_t * k, double a[2], double b[2] );
    };
    
    extern struct kinetic Kinetic;
    
void Camera_Rotation_Init( kinetic_t * k );

    
#ifdef __cplusplus
};
#endif
#endif /* kinetic_h */
