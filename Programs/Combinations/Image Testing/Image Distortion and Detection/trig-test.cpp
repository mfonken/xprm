//
//  trig-test.cpp
//  
//
//  Created by Matthew Fonken on 11/2/16.
//
//

#include <stdio.h>
#include <iostream>
#include <math.h>
#include <sys/time.h>
#include "types.h"
#include "sin_cos_atan.h"

#define STEP M_PI / 100
#define ab(x) x > 0 ? x:-x
using namespace std;

double s,c;


int main()
{
    clock_t t1, t2;
    double diff;
    t1 = clock();
    for(int x = -640; x <= 640; x++ )
    {
        for(int y = -32; y <= 32; y++ )
        {
            double sin_atan = sin(atan2(x, y));
            double cos_atan = cos(atan2(x, y));
            coord z;
            z.x = x;
            z.y = y;
            s_c_a(&z, &s, &c);
            //printf("(%4d,%4d)>\treal sin = %2.10f |  real cos = %2.10f\n\t       s_c_a sin = %2.10f | s_c_a cos = %2.10f\n", x, y, sin_atan, cos_atan, s, c );
        }
    }
    t2 = clock();
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
    printf("Time is %.3fms\n",diff);
}
