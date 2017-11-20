//
//  main.cpp
//  crtest
//
//  Created by Matthew Fonken on 11/17/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>
#include <math.h>

#include "cr.hpp"
#include "kinetic_types.h"

double scale = 100;

const double default_rotation[] = { 0.0, -40, 90.0 };
const int default_beaconA[] = {320, 600};
const int default_beaconB[] = {960, 200};

int main(int argc, const char * argv[]) {

    Camera_Rotation_Init();
    
    kinetic_t kin;
    for(int i = 0; i < 3; i++)
        kin.rotation[i] = default_rotation[i] * DEG_TO_RAD;
    cartesian2_t bea[2];
    bea[0].x = default_beaconA[0];
    bea[0].y = default_beaconA[1];
    bea[1].x = default_beaconB[0];
    bea[1].y = default_beaconB[1];
    
//    printf("Default case: ");
//    cr(&kin, bea);
//    printf("X: %.2f Y: %.2f Z: %.2f (cm) \n", kin.position[0] * scale, kin.position[1] * scale, kin.position[2] * scale);
    
    char name = 'A';
    for(int i = 45; name < 'H'; i+=15)
//    for(int i = 0; i <= 639 ; i++)
    {
        printf("%c: %4dº>", name++, i);
        kin.rotation[2] = i * DEG_TO_RAD;
        
//        bea[0].x = i;
//        bea[1].x = 1280-i;
//        printf("(%4d,%4d)<>(%4d,%4d): ", (int)bea[0].x, (int)bea[0].y, (int)bea[1].x, (int)bea[1].y);
        cr(&kin, bea);
        printf("\tX: %.4f Y: %.4f Z: %.4f (%s)\n", kin.position[0] * scale, kin.position[1] * scale, kin.position[2] * scale, UNITS);
    }
    
    return 0;
}
