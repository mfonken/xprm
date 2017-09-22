//
//  main.cpp
//  tau+
//
//  Created by Matthew Fonken on 8/23/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>

#include "global_types.h"
#include "tau_test.hpp"
#include "tau.h"
#include "rho.h"
#include <sys/time.h>

#define IMAGE_ROOT "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Algorithms/Image Processing/tau/tau+/tau+/images/"

#define FRAME_DELAY_MS 150

#define CAM_WIDTH   640
#define CAM_HEIGHT  320


using namespace cv;
using namespace std;

int main( int argc, char * args[] )
{
    struct timeval stop, start;
    bool no_file = true;
    Mat frameMat;
    int gif_loop = -1;
    int NUM_GIF_FRAMES = 1;
    if(argc == 2)
    {
        no_file = false;
        NUM_GIF_FRAMES = atoi(args[1]);
        printf("Using gif with %d frames.\n",NUM_GIF_FRAMES);
        gif_loop = NUM_GIF_FRAMES;
        std::string file(IMAGE_ROOT);
        file.append("frames/1.png");
        frameMat = imread(file, IMREAD_COLOR );
        printf("Starting with size %dx%d\n", frameMat.cols, frameMat.rows);
    }
    if(argc > 2)
    {
        std::string file(IMAGE_ROOT);
        file.append(args[1]);
        file.append(".");
        file.append(args[2]);
        printf("opening file: %s\n", file.c_str());
        frameMat = imread(file, IMREAD_COLOR );
        no_file = false;
        if( frameMat.empty() )                      // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return -1;
        }
    }
    
    int width = CAM_WIDTH, height = CAM_HEIGHT;
    VideoCapture cam(0);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    if (!cam.isOpened()) cout << "cannot open camera" << endl;
    
    if(no_file)
    {
        cam.read(frameMat);
        printf("Init-ing camera: (%d, %d)\n", frameMat.cols, frameMat.rows);
    }
//    if(frameMat.cols < width)  width  = frameMat.cols;
//    if(frameMat.rows < height) height = frameMat.rows;
    
    width  = frameMat.cols;
    height = frameMat.rows;
    
//    printf("Using frame size %dx%d.\n", width, height);
    
    pixel_base_t **frame;
    initImg(&frame, width, height);
    
    density_map_pair_t      d;
    peak_list_pair_t        p;
    probability_list_pair_t r;
    prediction_pair_t       e;

    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    cv::Point textOrg(3, 18);
    
    initTauA( width, height );

    for(int l=0;l<1;) {
        
        Mat out(height, width, CV_8UC3, Scalar(0,0,0));
        if(no_file) cam.read(frameMat);
        else if(gif_loop > 0)
        {
            gif_loop %= NUM_GIF_FRAMES;
            gif_loop++;
            std::string file(IMAGE_ROOT);
            file.append("frames/");
            file.append(to_string( gif_loop ));
            file.append(".png");
//            printf("Opening frame #%d.\n", gif_loop);
            frameMat = imread(file, IMREAD_COLOR );
        }
        
        MatToCimage(width, height, frameMat, frame);
        cimageToBandW( width, height, frame, out );

        /* Tau A Start */
        gettimeofday(&start, NULL);
        performTauA(frame, &e);
        gettimeofday(&stop, NULL);
        /* Tau A End */
        
        
        
        double seconds  = stop.tv_sec  - start.tv_sec;
        double useconds = stop.tv_usec - start.tv_usec;
        double diff = ((seconds) + useconds/1000000.0) + 0.0005;
        int fps  = 1/diff;
    
        string text = "FPS:" + to_string(fps);
//        cv::putText(out, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness,8);
        
        getRhoData(&d, &p, &r);
        drawTau(out, &d, &p, &r, &e);
        //if (waitKey(FRAME_DELAY_MS) >= 0) break;
        char c = waitKey(500000);
        if (c == 03) printf("Next frame.\n");
        else if (c == 02)
        {
            gif_loop -= 2;
            if( gif_loop < 0 ) gif_loop += NUM_GIF_FRAMES;
            printf("Last frame. %d\n ", gif_loop);
        }
    }
    return 0;
}
