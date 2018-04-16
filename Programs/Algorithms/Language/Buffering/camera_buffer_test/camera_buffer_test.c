//
//  camera_buffer_test.c
//  
//
//  Created by Matthew Fonken on 2/1/17.
//
//

#include <stdio.h>
#include <stdint.h>
#include "cam_controller.c"



int main( int agrc, char * args[] )
{
    printf("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
    FILE *file;
    file = fopen( "input.txt", "r" );
    int in;
    int i;
    while( in != 99 )
    {
        char c;
        uint8_t n;
        in = 0;
        fscanf(file, "%d", &in);
        //printf("%d\n", in);
        n = (uint8_t)in;
        i = Camera_Buffer( n );
        Camera_Check();
    }
    Beacon_Print();
    return 0;
}
