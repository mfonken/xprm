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

#define CAM_WIDTH   640
#define CAM_HEIGHT  320
#define THRESHOLD   230
#define BRIGHTNESS  100

using namespace cv;
using namespace std;

bool thresh(uint8_t t, Vec3d p)
{
    return p[0] > t;
}

void MatToCimage( uint16_t width, uint16_t height, Mat mat, pixel_base_t ** img )
{
    for(uint16_t y = 0; y < height; y++)
    {
        for(uint16_t x = 0; x < width; x++)
        {
            Vec3b pixel = mat.at<Vec3b>(y, x);
            img[y][x] = (thresh(THRESHOLD, pixel)) ? 1:0;
        }
    }
}
void cimageToMat( uint16_t width, uint16_t height, pixel_base_t ** img, Mat mat )
{
    for(uint16_t y = 0; y < height; y++)
    {
        for(uint16_t x = 0; x < width; x++)
        {
            Vec3b pixel;
            pixel[0] = img[y][x];
            pixel[1] = img[y][x];
            pixel[2] = img[y][x];
            mat.at<Vec3b>(y, x) = pixel;
        }
    }
}

void cimageToBandW( uint16_t width, uint16_t height, pixel_base_t ** img, Mat mat )
{
    for(uint16_t y = 0; y < height; y++)
    {
        for(uint16_t x = 0; x < width; x++)
        {
            Vec3b pixel;
            pixel[0] = img[y][x]*BRIGHTNESS;
            pixel[1] = img[y][x]*BRIGHTNESS;
            pixel[2] = img[y][x]*BRIGHTNESS;
            mat.at<Vec3b>(y, x) = pixel;
        }
    }
}

int main( int argc, char * args[] )
{
    struct timeval stop, start;
    bool no_file = true;
    Mat frameMat;
    if(argc > 1)
    {
        std::string file("/Users/matthewfonken/Desktop/marbl/xprm/Programs/Algorithms/Image Processing/tau/taukalman/taukalman/images/");
        file.append(args[1]);
        file.append(".bmp");
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
    
    if(no_file) cam.read(frameMat);
    printf("Init-ing camera: (%d, %d)\n", frameMat.cols, frameMat.rows);
    if(frameMat.cols < width)  width  = frameMat.cols;
    if(frameMat.rows < height) height = frameMat.rows;
    
    
    pixel_base_t **frame;
    initImg(&frame, width, height);
    
    
//    prediction_pair_t prediction;
    
    density_map_pair_t      d;
    peak_list_pair_t        p;
    probability_list_pair_t r;
    prediction_pair_t       e;
    
//    initDensityMapPair(     &d, width, height);
//    initPeaksListPair(      &p, width, height);
//    initProbabilityListPair(&r, width, height);
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 1;
    cv::Point textOrg(3, 18);
    
    initTauA( width, height );

    for(int l=0;l<1;) {
        
        
        Mat out(height, width, CV_8UC3, Scalar(0,0,0));
        if(no_file) cam.read(frameMat);
        
        MatToCimage(width, height, frameMat, frame);
        cimageToBandW( width, height, frame, out );
        
        gettimeofday(&start, NULL);
        performTauA(frame, &e);
        gettimeofday(&stop, NULL);
        getRhoData(&d, &p, &r);
        
        if (waitKey(1) >= 0) break;
        
        double seconds  = stop.tv_sec  - start.tv_sec;
        double useconds = stop.tv_usec - start.tv_usec;
        double diff = ((seconds) + useconds/1000000.0) + 0.0005;
        int fps  = 1/diff;
        
//        printf("fps: %f\n", diff);
        string text = "FPS:" + to_string(fps);
        cv::putText(out, text, textOrg, fontFace, fontScale, Scalar::all(255), thickness,8);
        
        drawTau(out, &d, &p, &r, &e);
    }
    return 0;
}
