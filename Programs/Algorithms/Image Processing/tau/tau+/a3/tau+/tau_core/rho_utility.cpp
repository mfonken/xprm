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
//    selectPeakListPair(p);
}

void Rho::perform( cimage_t * img, PredictionPair * p )
{
    image = *img;
    generateCenterOfMass(p);
    generateDensityMapFromCImageWithQuadrantMasses();
    getDensityMaxPair();
    updateDensityKalmanPair();
    filterDensityPair();
    analyzeDensityPair();
    selectPeakListPair(p);
    updatePredictions(p);
}

void Rho::generateCenterOfMass( PredictionPair * p )
{
    comX = ((int)(p->x.primary + p->x.secondary)) >> 1;
    comY = ((int)(p->y.primary + p->y.secondary)) >> 1;
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
    int h = height, w = width, d = 0, dR = 0;
    int * mapx = density_map_pair.x.map, * mapy = density_map_pair.y.map;
    density_map_pair.y.length = w;
    density_map_pair.x.length = h;
    
    memset(mapy, 0, sizeof(int) * w);
    memset(mapx, 0, sizeof(int) * h);
    
//    double mX = 0;//, mY = 0;
    for( int y = 0; y < h; y++ )
    {
        dR = 0;
        for( int x = 0; x < w; x++ )
        {
            d = x + y*h;
            int p = (unsigned char)image.pixels[d];
            if(p > 10)
            {
                dR      ++;
                mapy[x] ++;
            }
        }
        mapx[y] = dR;
        
//        if(dR > mX) mX = dR;
    }
//    density_map_pair.x.max = mX;
}


void Rho::generateDensityMapFromCImageWithQuadrantMasses()
{
    int h = height, w = width, d = 0, dR = 0, y, x;
    int * mapx = density_map_pair.x.map, * mapy = density_map_pair.y.map;
    density_map_pair.y.length = w;
    density_map_pair.x.length = h;
    
    int x_tog = 0, y_tog = 0;
    
    memset(Q, 0, sizeof(int) * 4);
    memset(mapy, 0, sizeof(int) * w);
    memset(mapx, 0, sizeof(int) * h);
    
    for( y = 0; y < comY; y++ )
    {
        for( x = 0, dR = 0; x < w; x++, x_tog=x<comX )
        {
            d = x + y*h;
            int p = (unsigned char)image.pixels[d];
            if(p > 10)
            {
                dR      ++;
                mapy[x] ++;
                Q[x_tog]++;
            }
        }
        mapx[y] = dR;
    }
    for( y = comY; y < h; y++ )
    {
        for( x = 0, dR = 0; x < w; x++)//, x_tog=x<comX )
        {
            d = x + y*h;
            int p = (unsigned char)image.pixels[d];
            if(p > 10)
            {
                dR      ++;
                mapy[x] ++;
//                Q[y_tog << 1 | x_tog]++;
            }
        }
        mapx[y] = dR;
    }
    
//    printf("Q[0]=%d Q[1]=%d\nQ[2]=%d Q[3]=%d\n", Q[0], Q[1], Q[2], Q[3]);
}

void Rho::getDensityMaxPair()
{
    getDensityMax(&density_map_pair.x);
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
    if(g < 0) g = 0;
    
    for( int i = 0; i < l; i++ )
    {
        c = d->map[i];
        if(c > (t+1)) c = t - punishf*(c - t);
        d->map[i] = (c < g)?0:c-g;
    }
}

void Rho::analyzeDensityPair()
{
    analyzeDensity( &density_map_pair.x, &peak_list_pair.x );
    analyzeDensity( &density_map_pair.y, &peak_list_pair.y );
}

void Rho::analyzeDensity( DensityMap * d, PeakList * p )
{
    int c, max = 0, l = d->length;
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
            if(c > max) max = c;
        }
        else if(has)
        {
            if(p->length < MAX_PEAKS_RHO)
            {
                p->map[p->length] = (int)mavg/cavg;
                p->den[p->length] = (int)cavg;
                p->max[p->length] = max;
                
                
                if(cavg < 0)
                    cavg = 0;
//                if(p->den[p->length] < 0) p->den[p->length] = 0;
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
    selectPeakList( 2*density_map_pair.x.variance, &peak_list_pair.x, &p->x );
    selectPeakList( 2*density_map_pair.y.variance, &peak_list_pair.y, &p->y );
}

void Rho::selectPeakList( double v, PeakList * p, Prediction * o )
{
    int l = p->length, denc = 0, locc = 0, maxc = 0;
    int den[] = {0,0,0}, loc[] = {0,0,0}, max[] = {0,0,0};
    for(int i = 0; i < l; i++)
    {
        denc = p->den[i];
        locc = p->map[i];
        maxc = p->max[i];
        if( denc > den[0] )
        {
            den[2] = den[1];
            loc[2] = loc[1];
            max[2] = max[1];
            den[1] = den[0];
            loc[1] = loc[0];
            max[1] = max[0];
            den[0] = denc;
            loc[0] = locc;
            max[0] = maxc;
        }
        else if( denc > den[1] )
        {
            den[2] = den[1];
            loc[2] = loc[1];
            max[2] = max[1];
            den[1] = denc;
            loc[1] = locc;
            max[1] = maxc;
        }
    }
    
    o->primary   = loc[0];
    o->secondary = loc[1];
    
    o->primary_probability   = ((double)max[0])/v;
    o->secondary_probability = ((double)max[1])/v;
    o->alternate_probability = ((double)max[2])/v;
    
    o->alternate_probability *= 2;
}
void Rho::updatePredictions( PredictionPair * p )
{
    int Ax = p->x.primary, Ay = p->y.primary, Bx = p->x.secondary, By = p->y.secondary;
    double  Axp = p->x.primary_probability, Bxp = p->x.secondary_probability,
            Ayp = p->y.primary_probability, Byp = p->y.secondary_probability;
    
    if(Ax > Bx)
    {
        p->x.primary    = Bx;
        p->x.secondary  = Ax;
        p->x.primary_probability = Bxp;
        p->x.secondary_probability = Axp;
    }
    
    if(Q[0] < Q[1] ^ Ay < By)
    {
        p->y.primary    = By;
        p->y.secondary  = Ay;
        p->y.primary_probability = Byp;
        p->y.secondary_probability = Ayp;
    }
}
