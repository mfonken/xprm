//
//  main.cpp
//  tau+
//
//  Created by Matthew Fonken on 8/23/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>

#include "utility_master.h"
#include "environment_master.h"
#include "tau_master.h"
#include "combine_master.h"

using namespace cv;
using namespace std;

#define FPS         30
#define FRAME_DELAY 1000/FPS

int main( int argc, const char * argv[] )
{
    ImageUtility utility("ImageUtility", "frames/small", 26, FNL_RESIZE_W, FNL_RESIZE_H);
    Tau tau("Tau", &utility, FNL_RESIZE_W, FNL_RESIZE_H);
    Combine combine("Combine", &tau, FNL_RESIZE_W, FNL_RESIZE_H);
    SerialWriter comm(SFILE, FILENAME);
    
    Environment env(&utility, 10);
    env.addTest(&tau, 60);
//    env.addTest(&combine, &comm, 20);
    
    env.start();
    usleep(100000);
    env.pause();
    
    pthread_mutex_lock(&utility.outframe_mutex);
    TauDraw drawer(&tau, utility.outframe);
    pthread_mutex_unlock(&utility.outframe_mutex);

    while(1)
    {
        pthread_mutex_lock(&utility.outframe_mutex);
        drawer.drawDensitiesOnFrame(utility.outframe);
        imshow("Outframe", drawer.frame);
        pthread_mutex_unlock(&utility.outframe_mutex);
        
        string xks = tau.rho.density_map_pair.x.kalman.toString();
        string yks = tau.rho.density_map_pair.y.kalman.toString();        
        Mat dataframe(34, 800, CV_8UC3, Scalar(245,245,245));
        putText(dataframe, xks, Point(0,12), FONT_HERSHEY_PLAIN, 1, Scalar(15,15,15));
        putText(dataframe, yks, Point(0,28), FONT_HERSHEY_PLAIN, 1, Scalar(15,15,15));
        imshow("Kalman Data", dataframe);
    
        switch(waitKey(FRAME_DELAY))
        {
            case ' ':
                if(env.status != LIVE) env.resume();
                else env.pause();
                break;
            case 's':
                env.pause();
                usleep(10000);
                tau.avg = 0;
                tau.count = 0;
                env.start();
                usleep(10000000);
                env.pause();
                printf("Tau averaged %fms for %d iterations\n", tau.avg*1000, tau.count);
                break;
            default:
                break;
        }
        
    }
    return 0;
}
