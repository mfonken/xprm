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

int main(int argc, char * argv[]) {
    VideoCapture cam(1);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, WIDTH);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, HEIGHT);
    
    printf("Running with User Control\n");
    
    for(int l=0;l<1;)
    {
        Mat frame,
//        in,
//        out(HEIGHT, WIDTH, CV_8UC3, Scalar(0,0,0)),
        out2(HEIGHT, WIDTH, CV_8UC3, Scalar(0,0,0)),
        out3(HEIGHT, HEIGHT, CV_8UC3, Scalar(0,0,0));
        
        cam >> frame;
        
        Rect rectCrop(
//                      0
                      (WIDTH-HEIGHT)/2
                      , 0, HEIGHT, HEIGHT);
        Mat in(frame, rectCrop);
        
        invfisheye(in, out3);
        imshow("Out3", out3);
        
//        invfisheye(frame, out2);
//        imshow("Out3", out2);
        
        waitKey(OUT_MDL);
    }
    
    return 0;
}

