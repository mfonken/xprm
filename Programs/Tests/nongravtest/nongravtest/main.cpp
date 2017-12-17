//
//  main.cpp
//  nongravtest
//
//  Created by Matthew Fonken on 11/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "kinetic/imu_stubs.h"
#include "kinetic/kinetic_types.h"
#include "kinetic/kinetic.h"
#include "kinetic/matrix.h"

using namespace std;

double ori[] = {0,0,0};
double acc[] = {0.7071,0,-0.7071};

LSM9DS1_t lsm;
kinetic_t kin;

#define OUT_FPS  120
#define OUT_UDL  1000000 / OUT_FPS
#define MAX_BUFFER 74

void * DATA_WR_THREAD( void *data )
{
    const char file_name[] = "/Users/matthewfonken/Desktop/out.txt";
    
    ofstream outfile;
    outfile.open(file_name, ofstream::out | ofstream::trunc);
    if (!outfile.is_open())
    {
        printf("Failed to open %s\n", file_name);
        while(1);
    }
    printf("Opened %s\n", file_name);
    int counter = 1;
    while(1)
    {
        outfile.open(file_name, ofstream::out | ofstream::trunc);
        char kin_packet[MAX_BUFFER];
        int l = sprintf(kin_packet, "n,%.4f,%.4f,%.4f\r\n", kin.nongrav[0], kin.nongrav[1], kin.nongrav[2]);
        
        outfile.write(kin_packet,l);
        outfile.close();
        
        if(++counter > OUT_FPS) counter = 1;
        usleep(OUT_UDL);
    }
}

int main(int argc, const char * argv[])
{
    pthread_t threads[1];
    
    printf("Starting Data Output thread.\n");
    int t1 = pthread_create(&threads[0], NULL, &DATA_WR_THREAD, NULL);
    if (t1) {
        cout << "Error:unable to create Data Output thread," << t1 << endl;
        exit(-1);
    }
    
    
    printf("Initializing IMU.\n");
    IMU_Init( &lsm );
    
    printf("Initializing Kinetic Utility.\n");
    Kinetic_Init( &lsm, &kin );
    
    while(1)
    {
        Kinetic_Update_Rotation( &lsm, &kin );
        
        /* Create a orientation vector and quaternion */
        ang3_t ov;
        ov.x = kin.rotation[0];
        ov.y = kin.rotation[1];
        ov.z = kin.rotation[2];
            
//        printf("<%3d, %3d, %3d>(º) | ", (int)(ov.x * RAD_TO_DEG), (int)(ov.y * RAD_TO_DEG), (int)(ov.z * RAD_TO_DEG));
        
        quaternion_t oq;
        Euler_To_Quaternion(&ov, &oq);
        
        /* Rotate acceleration vector by quaternion */
        vec3_t a,r;
        a.i = lsm.data.accel_raw[0];
        a.j = lsm.data.accel_raw[1];
        a.k = lsm.data.accel_raw[2];
        Rotate_Vector_By_Quaternion(&a, &oq, &r);
        
        /* Negate gravity: -(-1g) = +1g */
        r.k -= 1.0;
        
//        printf("(%.2f, %.2f, %.2f)\n", r.i, r.j, r.k);
        
        kin.nongrav[0] = r.i;
        kin.nongrav[1] = r.j;
        kin.nongrav[2] = r.k;
    }
    
    return 0;
}
