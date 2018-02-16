//
//  rho_utility.cpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "rho_utility.hpp"
//#include "rho.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace cv;

#define opsign(X,Y)     (X<0)^(Y<0)
#define rising(X,Y)    (X>=0)&(Y<0)
#define falling(X,Y)    (X<0)&(Y>=0)

#define opsignthresh(X,Y,T)    ((X*Y)<T)
#define risingthresh(X,Y,T)    ((X*Y)<T)&(Y<X)
#define fallingthresh(X,Y,T)   ((X*Y)<T)&(Y>X)

Rho::Rho( int width, int height ) : gaussian(DEFAULT_GAUSS_LEN), density_map_pair(width, height), frame(height, width, CV_8UC3, Scalar(0,0,0))
{
    printf("Initializing Rho Utility: %dx%d\n", width, height);
    this->width  = width;
    this->height = height;
    gaussian.generate( DEFAULT_GAUSS_LEN, DEFAULT_SIGMA);
    cimageInit(&image, width, height);
}

void Rho::perform( Mat M, PredictionPair * p )
{
    frame = M;
    generateDensityMap();
    getDensityMaxPair();
    updateDensityKalmanPair();
    filterDensityPair();
    analyzeDensityPair();
    selectPeakListPair(p);
}

void Rho::perform( cimage_t * img, PredictionPair * p )
{
    image = *img;
    generateDensityMapFromCImage();
    getDensityMaxPair();
    updateDensityKalmanPair();
    filterDensityPair();
    analyzeDensityPair();
    selectPeakListPair(p);
}

void Rho::generateDensityMap()
{
    int h = height, w = width;
    int * mapx = density_map_pair.x.map, * mapy = density_map_pair.y.map;
    density_map_pair.y.length = w;
    density_map_pair.x.length = h;
    
    memset(mapy, 0, sizeof(int) * w);
    memset(mapx, 0, sizeof(int) * h);

    Vec3b pixel = {0,0,0};
//    double mX = 0, mY = 0;
    for( int y = 0; y < h; y++ )
    {
        int dR = 0;
        for( int x = 0; x < w; x++ )
        {
            int p = (int)frame.at<Vec3b>(y,x)[2];
            if(p > 100)
            {
                dR      ++;
                mapy[x] ++;
            }
        }
        mapx[y] = dR;
        
//        if(dR > mX) mX = dR;
    }
}

void Rho::generateDensityMapFromCImage()
{
    int h = height, w = width, d = 0;
    int * mapx = density_map_pair.x.map, * mapy = density_map_pair.y.map;
    density_map_pair.y.length = w;
    density_map_pair.x.length = h;
    
    memset(mapy, 0, sizeof(int) * w);
    memset(mapx, 0, sizeof(int) * h);
    
    Vec3b pixel = {0,0,0};
    double mX = 0, mY = 0;
    for( int y = 0; y < h; y++ )
    {
        int dR = 0;
        for( int x = 0; x < w; x++ )
        {
            d = x + y*h;
            int p = (unsigned char)image.pixels[d];
            if(p)
            {
                dR      ++;
                mapy[x] ++;
            }
        }
        mapx[y] = dR;
        
        if(dR > mX) mX = dR;
    }
    density_map_pair.x.max = mX; 
}

void Rho::getDensityMaxPair()
{
//    getDensityMax(&density_map_pair.x);
    getDensityMax(&density_map_pair.y);
}
void Rho::getDensityMax( DensityMap * d )
{
    int l = d->length, c = 0, m = 0;
    for( int i = 1; i < l; i++, c = d->map[i] ) if(c > m ) m = c;
    d->max = m;
}

void Rho::updateDensityKalmanPair()
{
    updateDensityKalman( &density_map_pair.x );
    updateDensityKalman( &density_map_pair.y );
}

void Rho::updateDensityKalman( DensityMap * d )
{
    d->kalman.update(d->max, 0.);
    const double target = 0.15, normalv = 10, scalev = 20;
    d->variance = normalv * ( 1 + scalev*(target - d->kalman.K[0]) );
    //printf("mXv is %.3f & mYv is %.3f\n", mXv, mYv);}
}

void Rho::filterDensityPair()
{
    filterDensity( &density_map_pair.x );
    filterDensity( &density_map_pair.y );
}

void Rho::filterDensity( DensityMap * d )
{
    int c, t = d->kalman.value, v = d->variance, g = t - v, l = d->length, punishf = 5;
    for( int i = 0; i < l; i++ )
    {
        c = d->map[i];
        if(c > (t+1)) c = t - punishf*(c - t);
        d->map[i] = (c < g)?0:c;
    }
}

void Rho::analyzeDensityPair()
{
    analyzeDensity( &density_map_pair.x, &peak_list_pair.x );
    analyzeDensity( &density_map_pair.y, &peak_list_pair.y );
}

void Rho::analyzeDensity( DensityMap * d, PeakList * p )
{
    int c, l = d->length;
    p->length = 0;
    bool has = false;
    double cavg = 0, mavg = 0;
    int count = 0;
    for( int i = 0; i < l; i++ )
    {
        c = d->map[i];
        if(c)
        {
            count++;
            if(!has) has = true;
            cma((double)c, &cavg, count);
            cma((double)(c*i), &mavg, count);
        }
        else if(has)
        {
            if(p->length < MAX_PEAKS_RHO)
            {
                p->map[p->length] = mavg/cavg;
                p->den[p->length] = cavg;
                p->length++;
            }
            mavg = 0;
            cavg = 0;
            count = 0;
            has = false;
        }
    }
}

void Rho::selectPeakListPair( PredictionPair * p )
{
    selectPeakList( &peak_list_pair.x, &p->x );
    selectPeakList( &peak_list_pair.y, &p->y );
}

void Rho::selectPeakList( PeakList * p, Prediction * o )
{
    int l = p->length, denc = 0, den1 = 0, den2 = 0, locc = 0, loc1 = 0, loc2 = 0;
    for(int i = 0; i < l; i++)
    {
        denc = p->den[i];
        locc = p->map[i];
        if( denc > den1 )
        {
            den2 = den1;
            loc2 = loc1;
            den1 = denc;
            loc1 = locc;
        }
        else if( denc > den2 )
        {
            den2 = denc;
            loc2 = locc;
        }
    }
    if(loc1 < loc2)
    {
        o->primary   = loc1;
        o->secondary = loc2;
    }
    else
    {
        o->primary   = loc2;
        o->secondary = loc1;
    }
}
