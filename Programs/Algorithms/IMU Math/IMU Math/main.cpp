//
//  main.cpp
//  IMU Math
//
//  Created by Matthew Fonken on 12/18/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>

#include "kinetic/kinetic_master.h"

int main(int argc, const char * argv[]) {
    printf("Starting...\n");
    
    /* The IMU types fetches data from physical device and performs basic operations */
    imu_t imu;
    if(!IMU.init(&imu)) printf("Could not initialize IMU...\n");
    
    /* The Kinetic type filters and perform more complex operations using IMU data */
    kinetic_t kin;
    if(!Kinetic.init(&kin)) printf("Could not initialize kinetics...\n");
    
    
    
    return 0;
}
