//
//  kinetic_master.h
//  IMU Math
//
//  Created by Matthew Fonken on 12/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef kinetic_master_h
#define kinetic_master_h

//#include "imu_wrapper.h"
#include "point.h"
#include "kinetic.h"

typedef struct
{
    quaternion_t data;
    floating_t confidence; /// TODO: Incorporate confidence
    uint32_t timestamp;
} orientation_data_t;

typedef struct
{
    kpoint_t data[3];
    floating_t confidence[3];
    uint32_t timestamp;
} rho_data_t;

#endif /* kinetic_master_h */
