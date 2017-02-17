//
//  kinetic_test.cpp
//  
//
//  Created by Matthew Fonken on 1/22/17.
//
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "kalman.c"

#define RATE_FACTOR 0
#define SCALE       100
#define SKIP        1

void generate_data( FILE * file, int l )
{
    fseek(file, 0, SEEK_SET);
    
    /* initialize random seed: */
    srand (time(NULL));
    
    int p = rand() % SCALE;
    printf("Generating %d noisy data points.\n", l);
    for( int i = 0; i < l; i++)
    {
        int r = rand() % SCALE;
        r -= SCALE / 2;
        int c = p + r;
        fprintf(file, "%d, %d\n", i, c);
        p = c;
    }
    printf("Data generated.\n");
}

void filter_data( FILE * file, int l )
{
    fseek(file, 0, SEEK_SET);
    
    FILE * out;
    out = fopen("filtered_sample.csv", "w+");
    fseek(out, 0, SEEK_SET);
    int x, y;
    fscanf(file, "%d, %d", &x, &y);
    
    //printf("First value is %d\n", x);
    
    kalman_t filter;
    Kalman_Init( &filter, (double)y);
    
    double p = (double)y;
    
    for( int i = 1; i < l; i += SKIP)
    {
        fscanf(file, "%d, %d", &x, &y);
        //printf("%d>%d\n", x, y);
        
        double r =  ( (double)y - p ) * RATE_FACTOR;
        p = y;
        
        Kalman_Update( &filter, (double)y, r, SKIP );
        fprintf(out, "%d, %d, %lf\n", i, y, filter.value);
    }
    fclose(out);
}

int main( int argc, char * args[] )
{
    FILE *fp;
    if(argc >= 2)
        fp=fopen(args[1], "a+");
    else
        fp=fopen("samples.csv", "w+");

    int l = 10;
    if(argc == 3)
        l = atoi(args[2]);

    generate_data(fp, l);
    
    //fclose(fp);
    //fp=fopen("samples.csv", "w+");
    
    filter_data(fp, l);
    fclose(fp);
    return 0;
}

