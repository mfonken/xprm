//
//  cordic-test.c
//  
//
//  Created by Matthew Fonken on 11/1/16.
//
//

#include <stdio.h>
#include "cordic.h"
#include <math.h> // for testing only!
#include <iostream>
#include <sys/time.h>
#include "sin_cos_atan.h"

#define STEPS 90.0
#define POW   12

#define ab(x) x > 0 ? x:-x

using namespace std;

double quadrant_f[4] = { 0, M_PI/2, -M_PI, - M_PI/2 };

int sin_errors[(int)STEPS * 4];
int cos_errors[(int)STEPS * 4];


//Print out sin(x) vs fp CORDIC sin(x)
int main(int argc, char **argv)
{
    double p;
    double s,c;
    int i;
    clock_t t1, t2;
    double diff, ret;
    for(int q = 0; q < 4; q++ )
    {
        for(i=0;i<STEPS;i++)
        {
            p = (i/STEPS)*M_PI/2 + quadrant_f[q];
            ret = sin(p);
            ret = cos(p);
        }
    }
    for(int r = 1; r <= 32; r++)
    {
        
        t1 = clock();
        for(int q = 0; q < 4; q++ )
        {
            for(i=0;i<STEPS;i++)
            {
                p = (i/STEPS)*M_PI/2 + quadrant_f[q];
                double ret = cordic(p, &s, &c, r);
            }
        }
        t2 = clock();
        diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
        printf("For precision %2d, time is %.3fms\n", r,diff);
        
        t1 = clock();
        for(int q = 0; q < 4; q++ )
        {
            for(i=0;i<STEPS;i++)
            {
                p = (i/STEPS)*M_PI/2 + quadrant_f[q];
                ret = sin(p);
                ret = cos(p);
            }
        }
        t2 = clock();
        diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
        printf("For std math, time is     %.3fms\n", diff);
        
        t1 = clock();
        for(int q = 0; q < 4; q++ )
        {
            for(i=0;i<STEPS;i++)
            {
                p = (i/STEPS)*M_PI/2 + quadrant_f[q];
                coord z;
                z.x = p;
                z.y = 1;
                s_c_a( &z, &s, &c);
            }
        }
        t2 = clock();
        diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
        printf("For non-trig, time is     %.3fms\n", diff);
        
    }
 }


//sin_errors[i*q] = ab(((sin(p) - s) * pow(10,POW) ) / sin(p));
//cos_errors[i*q] = ab(((cos(p) - c) * pow(10,POW) ) / cos(p));
//cout << "Return is " << ret << " on precision " << r << endl;
//these values should be nearly equal
//                printf("%.0fº : %.0fº > %f : %f | %f : %f\n", p*180/M_PI, ret*180/M_PI, s, sin(p), c, cos(p));
//                cout << "Sin error is " << sin_errors[i*q] << endl;
//                cout << "Cos error is " << cos_errors[i*q] << endl;

//        double sin_error = 0;
//        double cos_error = 0;
//        for(int i = 0; i < STEPS * 4; i++)
//        {
//            sin_error += sin_errors[i] / pow(10,POW);
//            cos_error += cos_errors[i] / pow(10,POW);
//        }
//        sin_error /= ( STEPS * 4 );
//        cos_error /= ( STEPS * 4 );
//
//        printf(" and error sin > %.8f%% and cos > %.8f%%\n", sin_error * 100, cos_error * 100);
