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
#define     CAMERA_ALPHA_H          75      // degrees

#define     CAMERA_WIDTH            1280    // pixels
#define     CAMERA_HEIGHT           800     // pixels
#define     CAMERA_HALF_WIDTH       CAMERA_WIDTH/2
#define     CAMERA_HALF_HEIGHT      CAMERA_HEIGHT/2


#define     UNITS                  "cm"
#define     PIXEL_TO_UNIT           3e-6
#define     UNIT_TO_PIXEL         ( 1.0 / PIXEL_TO_UNIT )

#define     FOCAL_REFRACTION        1.42e-6//0.945e-6  // units / units
#define     FOCAL_LENGTH          ( 3.5e-3 * FOCAL_REFRACTION ) * UNIT_TO_PIXEL // dimension units

#define     D_FIXED                 3.4e-2

void qCT();
void cr( kinetic_t * kinetics, cartesian2_t beacons[2] );
void Camera_Rotation_Init();
#endif /* cr_hpp */
