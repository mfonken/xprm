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
#include "tau.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 320
#define THRESHOLD 245

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
            pixel[0] = img[y][x]*255;
            pixel[1] = img[y][x]*255;
            pixel[2] = img[y][x]*255;
            mat.at<Vec3b>(y, x) = pixel;
        }
    }
}

void print2DArray( pixel_base_t ** i, int w, int h )
{
//    pixel_base_t f[w][h];
    for(int y=0; y<h; y++)
    {
        for(int x=0; x<w; x++)
        {
            pixel_base_t p = i[y][x];
            printf("(%d, %d) is %d\n",x,y,p);
        }
    }
}

void initImg( pixel_base_t *** i, int w, int h )
{
    *i = (pixel_base_t**)malloc(sizeof(pixel_base_t*) * h);
    for(int y=0;y<h;y++) (*i)[y] = (pixel_base_t*)malloc(sizeof(pixel_base_t) * w);
}



//void initPredictionPair( prediction_pair_t * e)
//{
//    (*e).x = (prediction_t)malloc(sizeof(prediction_t));
//    (*e).y = (prediction_t)malloc(sizeof(prediction_t));
//}

void drawDensityMap(density_map_pair_t * m)
{
    int w = m->x.length;
    int h = m->y.length;
    
    int draw_height = 200;
    
    Mat Xd(draw_height, w, CV_8UC3, Scalar(0,0,0));
    Mat Yd(h, draw_height, CV_8UC3, Scalar(0,0,0));
    
    Vec3b p;
    p[0] = 255;
    p[1] = 0;
    p[2] = 255;
    int d = 0;
    
    for(int x=0; x<w; x++)
    {
        d = m->x.map[x];
        if(d >= draw_height) d = draw_height-1;
        Xd.at<Vec3b>(draw_height-1-d,x) = p;
    }
    for(int y=0; y<h; y++)
    {
        d = m->y.map[y];
        if(d >= draw_height) d = draw_height-1;
        Yd.at<Vec3b>(y,draw_height-1-d) = p;
    }
    imshow("X Density Map", Xd);
    imshow("Y Density Map", Yd);
}

void drawDensityMapWithPeaks(density_map_pair_t * m, peak_list_pair_t * p)
{
    int w = m->x.length;
    int h = m->y.length;
    
    int draw_height = 300;
    
    Mat Xd(draw_height, w, CV_8UC3, Scalar(0,0,0));
    Mat Yd(h, draw_height, CV_8UC3, Scalar(0,0,0));
    
    Vec3b mp;
    mp[0] = 255;
    mp[1] = 0;
    mp[2] = 255;
    
    Vec3b pp;
    pp[0] = 0;
    pp[1] = 0;
    pp[2] = 255;
    
    
    int d = 0;
    
    for(int x=0; x<w; x++)
    {
        d = m->x.map[x];
        if(d >= draw_height) d = draw_height-1;
        Xd.at<Vec3b>(draw_height-1-d,x) = mp;
    }
    for(int i=0; i<p->x.length; i++)
    {
        int l = p->x.locations[i];
        for(int z = 0; z < draw_height; z++)
            Xd.at<Vec3b>(z,l) = pp;
    }
    for(int y=0; y<h; y++)
    {
        d = m->y.map[y];
        if(d >= draw_height) d = draw_height-1;
        Yd.at<Vec3b>(y,draw_height-1-d) = mp;
    }
    for(int j=0; j<p->y.length; j++)
    {
        int l = p->y.locations[j];
        for(int z = 0; z < draw_height; z++)
            Yd.at<Vec3b>(l,z) = pp;
    }
    imshow("X Density Map", Xd);
    imshow("Y Density Map", Yd);
}

void drawDensityMapWithProbabilities(density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r)
{
    int h = m->x.length;
    int w = m->y.length;
    int draw_height = 300;
    
    printf("(%d, %d)\n",w,h);
    
    Mat Xd(h, draw_height, CV_8UC3, Scalar(0,0,0));
    Mat Yd(draw_height, w, CV_8UC3, Scalar(0,0,0));
    
    Vec3b mp;
    mp[0] = 255;
    mp[1] = 0;
    mp[2] = 255;
    
    Vec3b pp;
    pp[0] = 0;
    pp[1] = 0;
    pp[2] = 255;
    
    int d = 0;
    
    for(int x=0; x<h; x++)
    {
        d = m->x.map[x];
        if(d >= draw_height) d = draw_height-1;
        Xd.at<Vec3b>(x,draw_height-1-d) = mp;
    }
    for(int i=0; i<p->x.length; i++)
    {
        int l = p->x.locations[i];
        FLOAT v = r->x.primary[i]*3;
        if(v > 1) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < draw_height*v; z++)
            Xd.at<Vec3b>(l,draw_height-1-z) = rp;
    }
    for(int y=0; y<w; y++)
    {
        d = m->y.map[y];
        if(d >= draw_height) d = draw_height-1;
        Yd.at<Vec3b>(d,y) = mp;
    }
    for(int i=0; i<p->y.length; i++)
    {
        int l = p->y.locations[i];
        FLOAT v = r->y.primary[i]*3;
        if(v > 1) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < draw_height*v; z++)
            Yd.at<Vec3b>(z, l) = rp;
    }
    imshow("X Density Map", Xd);
    imshow("Y Density Map", Yd);
}

void drawDensityMapWithPredictions(density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r, prediction_pair_t * e)
{
    int h = m->x.length;
    int w = m->y.length;
    int draw_height = 300;
    
//    printf("(%d, %d)\n",e->x.primary,e->x.primary);
    
    Mat Xd(h, draw_height, CV_8UC3, Scalar(0,0,0));
    Mat Yd(draw_height, w, CV_8UC3, Scalar(0,0,0));
    
    Vec3b mp;
    mp[0] = 255;
    mp[1] = 0;
    mp[2] = 255;
    
    Vec3b pp;
    pp[0] = 0;
    pp[1] = 0;
    pp[2] = 255;
    
    Vec3b ep;
    ep[0] = 255;
    ep[1] = 0;
    ep[2] = 0;
    
    int d = 0;
    
    int xp = e->x.primary;
    int xs = e->y.secondary;
    int yp = e->x.primary;
    int ys = e->y.secondary;
    
    for(int x=0; x<h; x++)
    {
        d = m->x.map[x];
        if(d >= draw_height) d = draw_height-1;
        Xd.at<Vec3b>(x,draw_height-1-d) = mp;
    }
    for(int i=0; i<p->x.length; i++)
    {
        int l = p->x.locations[i];
        FLOAT v = r->x.primary[i]*3;
        if(v > 1) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < draw_height*v; z++)
            Xd.at<Vec3b>(l,draw_height-1-z) = rp;
    }
    
    for(int z=0; z<draw_height; z++)
    {
        Xd.at<Vec3b>(xp,z) = ep;
        Xd.at<Vec3b>(xs,z) = ep;
    }
    for(int y=0; y<w; y++)
    {
        d = m->y.map[y];
        if(d >= draw_height) d = draw_height-1;
        Yd.at<Vec3b>(d,y) = mp;
    }
    for(int i=0; i<p->y.length; i++)
    {
        int l = p->y.locations[i];
        FLOAT v = r->y.primary[i]*3;
        if(v > 1) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < draw_height*v; z++)
            Yd.at<Vec3b>(z, l) = rp;
    }
    for(int z = 0; z < draw_height; z++)
    {
        Yd.at<Vec3b>(z,yp) = ep;
        Yd.at<Vec3b>(z,ys) = ep;
    }
    
    imshow("X Density Map", Xd);
    imshow("Y Density Map", Yd);
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
    if(testFrame.cols < width) width = testFrame.cols;
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
        imshow("cam", out);
        
        getDensityMap(frame, &d);
        getPeakList(&d, &p);
        getProbabilityList(&p, &r);
        getPredictions(&p, &r, &e);

        drawDensityMapWithPredictions(&d, &p, &r, &e);
        
        if (waitKey(30) >= 0)
            break;
    }
    return 0;
}
