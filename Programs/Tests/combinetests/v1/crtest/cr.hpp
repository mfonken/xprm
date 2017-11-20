//
//  cr.hpp
//  crtest
//
//  Created by Matthew Fonken on 11/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef cr_hpp
#define cr_hpp

#include <stdio.h>

#include "kinetic_types.h"
#include "matrix.h"

#define     CAMERA_OFFSET_ANGLE_X   0       // degrees
#define     CAMERA_OFFSET_ANGLE_Y   0       // degress
#define     CAMERA_OFFSET_ANGLE_Z   0       // degrees

#define     CAMERA_ALPHA_W          120     // degrees
#define     CAMERA_ALPHA_H          90      // degrees

#define     CAMERA_WIDTH            1280    // pixels
#define     CAMERA_HEIGHT           800     // pixels

//#define     SCALE                   1000

#define     PIXEL_TO_METER          3e-6
#define     METER_TO_PIXEL        ( 1.0 / PIXEL_TO_METER )

#define     FOCAL_REFRACTION        1.5e-6
#define     FOCAL_LENGTH          ( 3.5e-3 * FOCAL_REFRACTION ) * METER_TO_PIXEL // dimension units

#define     D_FIXED                 3.4e-2

void cr( kinetic_t * kinetics, cartesian2_t beacons[2] );
void Camera_Rotation_Init();
#endif /* cr_hpp */
