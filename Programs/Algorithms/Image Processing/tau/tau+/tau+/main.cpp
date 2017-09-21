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

#include "global_types.h"
#include "tau_test.hpp"
#include "tau.h"

#define CAM_WIDTH   640
#define CAM_HEIGHT  320
#define THRESHOLD   200
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

int main()
{
    int width = CAM_WIDTH, height = CAM_HEIGHT;
    VideoCapture cam(0);
    cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
    cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
    if (!cam.isOpened()) cout << "cannot open camera" << endl;
    
    Mat testFrame;
    cam.read(testFrame);
    printf("Init-ing camera: (%d, %d)\n", testFrame.cols, testFrame.rows);
    if(testFrame.cols < width)  width  = testFrame.cols;
    if(testFrame.rows < height) height = testFrame.rows;
    
    
    pixel_base_t **frame;
    initImg(&frame, width, height);
    
    initTauA( width, height );
//    prediction_pair_t prediction;
    
    density_map_pair_t d;
    initDensityMapPair(&d, width, height);
    peak_list_pair_t p;
    initPeaksListPair(&p, width, height);
    probability_list_pair_t r;
    initProbabilityListPair(&r, width, height);
    prediction_pair_t e;

    for(int l=0;l<1;) {
        Mat frameMat, out(height, width, CV_8UC3, Scalar(0,0,0));;
        
        cam.read(frameMat);
        
        MatToCimage(width, height, frameMat, frame);
        cimageToBandW( width, height, frame, out );
//        imshow("cam", out);
        
        getDensityMap(frame, &d);
        smooth1D(d.x.map, d.x.length);
        smooth1D(d.y.map, d.y.length);
        getPeakList(&d, &p);
        getProbabilityList(&p, &r);
        getPredictions(&p, &r, &e);

        drawTau(out, &d, &p, &r, &e);
        
        if (waitKey(30) >= 0)
            break;
        
    }
    return 0;
}
