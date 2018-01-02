//
//  main.cpp
//  fisheyetest
//
//  Created by Matthew Fonken on 12/25/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sys/time.h>

#include "unfisheye.h"
#include "open.hpp"

#define CAM_WIDTH               1920
#define CAM_HEIGHT              1080
#define FNL_RESIZE_S            2
#define WIDTH                   CAM_WIDTH/FNL_RESIZE_S
#define HEIGHT                  CAM_HEIGHT/FNL_RESIZE_S

#define TEXT_OFFSET_X -26
#define TEXT_OFFSET_Y  18
#define DETECT_BORDER_OFFSET 10

#define OUT_FPS  1000
#define OUT_MDL  1000 / OUT_FPS
#define OUT_UDL  1000000 / OUT_FPS

using namespace cv;
using namespace std;

double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0) + 0.0005; }

#ifdef PERFORMANCE_TEST
int orders[] = { 10, 100, 500 };//1000, 10000, 100000 };
int num_orders = sizeof(orders)/sizeof(orders[0]);
double t[3];
struct timeval start,stop;
#endif

int main(int argc, char * argv[]) {
    VideoCapture cam(0);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    
    //    double f = 0.0;
    //    int counter = 1;
    
    open_t tra;
    std::vector<KeyPoint> kps;
    
    printf("Running with User Control\n");
    
#ifdef PERFORMANCE_TEST
    for(int o = 0; o < num_orders; o++)
    {
        int iterations = orders[o];
        double times[iterations];
        printf("Running for %d iterations\n", iterations);
        for(int l=0;l<iterations;l++)
        {
#else
            for(int l=0;l<1;)
            {
#endif
            
            Mat frame, in, out(HEIGHT, WIDTH, CV_8UC3, Scalar(0,0,0)), detect;
            cam >> frame;
//            Rect rectCrop((WIDTH-HEIGHT)/2, 0, HEIGHT, HEIGHT);
//            Mat in(frame, rectCrop);
            resize(frame, in, Size(WIDTH,HEIGHT), 1, 1);
                
#ifdef PERFORMANCE_TEST
            gettimeofday( &start, NULL);
#endif
            /*** Fisheye Correction Algorithm ***/
//            quickUnfisheye(in, out);
            unfisheye(in, out);
                
            /************************************/
                
                kps = tra.detect(out, detect);
                
                vector<KeyPoint> gkps;
                for(int i = 0; i < kps.size(); i++)
                {
                    Point pt(kps.at(i).pt);
                    if(   pt.x > DETECT_BORDER_OFFSET && pt.x < ( WIDTH - DETECT_BORDER_OFFSET)
                       && pt.y > DETECT_BORDER_OFFSET && pt.y < ( HEIGHT - DETECT_BORDER_OFFSET))
                    {
                        Point opt(pt.x + TEXT_OFFSET_X, pt.y + TEXT_OFFSET_Y);
                        putText(out, "+", opt, FONT_HERSHEY_SCRIPT_SIMPLEX, 2, Vec3b(0,0,255), 3, 8);
                        gkps.push_back(kps.at(i));
                    }
                }

#ifdef PERFORMANCE_TEST
            gettimeofday( &stop, NULL);
            times[l] = timeDiff(start, stop);
#endif
            
            ////        printf("Counter: %d\n", counter);
            //        if(++counter > OUT_MDL)
            //        {
            //            counter = 1;
            //            f += 0.01;
            //        }
            //
                
#ifndef PERFORMANCE_TEST
            imshow("In", in);
            imshow("Out", out);
            waitKey(OUT_MDL);
#endif
        }
#ifdef PERFORMANCE_TEST
        long double average = 0;
        for(int l = 0; l < iterations; l++) average += times[l];
        average /= iterations;
        printf("L-%d A-%Lf\n", iterations, average);
    }
#endif
    return 0;
}

