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

#define STR_ANG 0
#define STP_ANG 180
#define STEP_AB 45

#define ANG_DIF (STP_ANG-STR_ANG)
#define STEP    ANG_DIF>0?STEP_AB:-STEP_AB

#define ROT_R  0
#define ROT_P  1
#define ROT_Y  1

#define A

double ori[] = {0,0,0};
double acc[] = {0.7071,0,-0.7071};

int main(int argc, const char * argv[])
{
    int ang_diff = ANG_DIF, step = STEP;
    int step_n = ang_diff/step;
    printf("Start%d Stop%d AngDif%d StepAb%d Step%d StepN%d\n", STR_ANG, STP_ANG, ang_diff, STEP_AB, step, step_n);
    printf("Testing with acceleration of: (%.2f, %.2f, %.2f) (g)\n", acc[0], acc[1], acc[2]);
    
    for(int i = 0, s = STR_ANG; i <= step_n; i++, s+=STEP)
    {
        printf("#%3d(%3dº): ", i, s);
        
        /* Create a orientation vector and quaternion */
        ang3_t ov;
        ov.x = ( ori[0] + ROT_R*s ) * DEG_TO_RAD;
        ov.y = ( ori[1] + ROT_P*s ) * DEG_TO_RAD;
        ov.z = ( ori[2] + ROT_Y*s ) * DEG_TO_RAD;
            
        printf("<%3d, %3d, %3d>(º): ", (int)(ov.x * RAD_TO_DEG), (int)(ov.y * RAD_TO_DEG), (int)(ov.z * RAD_TO_DEG));
        
        quaternion_t oq;
        Euler_To_Quaternion(&ov, &oq);
        
        /* Rotate acceleration vector by quaternion */
        vec3_t a,r;
        a.i = acc[0];
        a.j = acc[1];
        a.k = acc[2];
        Rotate_Vector_By_Quaternion(&a, &oq, &r);
        
        /* Negate gravity: -(-1g) = +1g */
        r.k += 1.0;
        
        printf("Non-grav acceleration is <%.2f, %.2f, %.2f>\n", r.i, r.j, r.k);
    }
    
    return 0;
}
