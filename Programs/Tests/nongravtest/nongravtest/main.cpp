//
//  main.cpp
//  nongravtest
//
//  Created by Matthew Fonken on 11/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>

#include "kinetic_types.h"
#include "matrix.h"

double pr[] = {0,90};
double accel[] = {0,0,-1};

int main(int argc, const char * argv[])
{
    printf("Testing with a pitch of %.2fº and a roll of %.2fº and starting accel of <%.2f, %.2f, %.2f>\n\n", pr[0], pr[1], accel[0], accel[1], accel[2]);
    quaternion_t q;
    
    /* Create a vector of accelerometer values */
    ang3_t a;
    a.x = pr[0] * DEG_TO_RAD;// imu->data.pitch;
    a.y = pr[1] * DEG_TO_RAD;// imu->data.roll;
    a.z = 0;
    Euler_To_Quaternion(&a, &q);
    
    vec3_t g,r,ngacc;
    g.i =  0;
    g.j =  0;
    g.k = -1;
    Rotate_Vector_By_Quaternion(&g, &q, &r);
    
    ngacc.i = accel[0] - r.i;
    ngacc.j = accel[1] - r.j;
    ngacc.k = accel[2] - r.k;
    
    printf("Non-grav acceleration is <%.2f, %.2f, %.2f>\n\n", ngacc.i, ngacc.j, ngacc.k);
    
    return 0;
}
