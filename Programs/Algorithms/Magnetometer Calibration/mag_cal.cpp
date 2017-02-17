//
//  mag_cal.c
//  
//
//  Created by Matthew Fonken on 12/21/16.
//
//

#include "mag_cal.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int main( int argc, char * args[])
{
    FILE *fp;
    //printf("%d\n", argc);
    if(argc >= 2)
        fp=fopen(args[1], "r+");
    else
        fp=fopen("sample.h", "r+");
    
    mag_cal_t samples;
    int k = 10;
    //fscanf(fp, "%i", &k);
    if(argc == 3)
        k = atoi(args[2]);
    printf("Performing calibration using %i samples.\n", k);
    samples.n = k;
    for( int i = 0; i < k; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            fscanf(fp, "%lf", &(samples.s[i][j]));
            printf("%lf ", samples.s[i][j]);
        }
        printf("\n");
    }

    double ret[4];
    performCalibration( ret, &samples );
    printf("Hard-iron Offset> (%.3lf, %.3lf, %.3lf)|%.3lf\n",ret[0], ret[1], ret[2], ret[3]);
    fclose(fp);
}
