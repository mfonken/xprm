//
//  tau_test.cpp
//  tau+
//
//  Created by Matthew Fonken on 8/27/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "tau_test.hpp"

using namespace cv;

Vec3b mp = { 255, 0, 255};
Vec3b pp = { 0, 0, 255};
Vec3b ap = { 255, 0, 0};
Vec3b bp = { 0, 255, 0};

void drawTau(Mat M, density_map_pair_t * m, peak_list_pair_t * p, probability_list_pair_t * r, prediction_pair_t * e)
{
//    printf("dens: %d x %d\n", m->x.length, m->y.length);
//    printf("peak: %d x %d\n", p->x.length, p->y.length);
//    printf("prob: %d x %d\n", r->x.length, r->y.length);
    
    int h       = m->x.length;
    int w       = m->y.length;
    int width   = M.rows;
    int height  = M.cols;
    
    if( h > width )  h = width;
    if( w > height ) w = height;
    
    FLOAT x_scale = 0.5;
    FLOAT y_scale = 0.5;
    
    int d = 0;
    
    int xp = e->x.primary;
    int xs = e->x.secondary;
    int yp = e->y.primary;
    int ys = e->y.secondary;
    
    for(int x=0; x<h; x++)
    {
        d = m->x.map[x] * x_scale * 2;
        if(d >= height) d = height-1;
        M.at<Vec3b>(x,d) = mp;
    }
    for(int i=0; i<p->x.length; i++)
    {
        int l = p->x.locations[i];
        FLOAT v = r->x.primary[i];
        v -= 0.6;
        v *= 3;
        if(v > 1 || v < 0) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < height*v*x_scale; z++)
            M.at<Vec3b>(l,z) = rp;
    }
//    printf("\t\t\t X: pri>%d & sec>%d\n", xp, xs);
    for(int z=0; z<height; z++)
    {
        M.at<Vec3b>(xp,z) = ap;
        M.at<Vec3b>(xs,z) = bp;
    }
    for(int y=0; y<w; y++)
    {
        d = m->y.map[y] * y_scale * 2;
        if(d >= width) d = width-1;
        M.at<Vec3b>(d,y) = mp;
    }
    for(int i=0; i<p->y.length; i++)
    {
        int l = p->y.locations[i];
        FLOAT v = r->y.primary[i];
        v -= 0.6;
        v *= 3;
        if(v > 1 || v < 0) break;
        Vec3b rp;
        rp[0] = 0;
        rp[1] = 255*(1-v);
        rp[2] = 255*v;
        for(int z = 0; z < width*v*y_scale; z++)
            M.at<Vec3b>(z, l) = rp;
    }
//     printf("\t\t\t Y: pri>%d & sec>%d\n", yp, ys);
    for(int z = 0; z < width; z++)
    {
        M.at<Vec3b>(z,yp) = ap;
        M.at<Vec3b>(z,ys) = bp;
    }
    imshow("   ", M);
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
