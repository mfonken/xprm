//
//  moment_utility.cpp
//  momentdensity
//
//  Created by Matthew Fonken on 2/3/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "moment_utility.hpp"

#define INR(X,H)    INRLH(X,0,H)
#define INRLH(X,L,H) (X>=H)?H-1:((X<L)?L:X)

static void cma( double new_val, double *avg, uint16_t num ) { *avg += ( new_val - *avg ) / ( num + 1 ); }

using namespace cv;
using namespace std;

MomentUtility::MomentUtility(Mat M)
{
    w = M.cols; h = M.rows;
    W = w + SIDEBAR_WIDTH; H = h + SIDEBAR_WIDTH;
    Mat N(W, H, CV_8UC3, Scalar(0));
    M.copyTo(N(Rect(0,0,w,h)));
    frame = N;
    
    dY = (density_t *)malloc(sizeof(density_t)*w);
    dX = (density_t *)malloc(sizeof(density_t)*h);
    fY = (density_t *)malloc(sizeof(density_t)*w);
    fX = (density_t *)malloc(sizeof(density_t)*h);
    bX = (density_t *)malloc(sizeof(density_t)*MAX_BLOBS);
    bY = (density_t *)malloc(sizeof(density_t)*MAX_BLOBS);
    
    mXp = mYp = mXv = mYv = 0;
    
    xk.init(mX);
    yk.init(mY);
}

void MomentUtility::updateFrame(Mat M)
{
    image = M;
    M.copyTo(frame(Rect(0,0,w,h)));
}

void MomentUtility::updateDensities(Mat M)
{
    updateFrame(M);
    getDensities();
    getMaxDensities();
    updateDensityKalmans();
    filterDensities();
    analyzeBlobs();
    drawDensitiesOnFrame();
}

void MomentUtility::getDensities()
{
    for( int x = 0; x < w; x++ ) dY[x] = 0;
    for( int y = 0; y < h; y++ )
    {
        density_t dR = 0;
        for( int x = 0; x < w; x++ )
        {
            density_t p = (density_t)pixelDensity(image.at<Vec3b>(y,x));
            dR    += p;
            dY[x] += p;
        }
        dX[y] = dR;
    }
}

void MomentUtility::getMaxDensities()
{
    mXp = mX; mYp = mY;
    mX = mY = 0;
    for( int y = 1; y < h; y++ )
        if( dX[y] > mX ) mX = dX[y];
    
    for( int x = 1; x < w; x++ )
        if( dY[x] > mY ) mY = dY[x];
}

void MomentUtility::updateDensityKalmans()
{
    xk.update(mX, 0.);//mX-xk.value);
    yk.update(mY, 0.);//mY-yk.value);
    
    const double xtarget = 0.25, ytarget = 0.25, normalv = 8, scalev = 20;
    mXv = normalv * ( 1 + scalev*(xtarget - xk.K[0]) );
    mYv = normalv * ( 1 + scalev*(ytarget - yk.K[0]) );
    //printf("mXv is %.3f & mYv is %.3f\n", mXv, mYv);
    
    string xks = xk.toString();
    string yks = yk.toString();
//    printf("XK: %s\n", xks.c_str());
    
    Mat dataframe(34, 800, CV_8UC3, Scalar(245,245,245));
    putText(dataframe, xks, Point(0,12), FONT_HERSHEY_PLAIN, 1, Scalar(15,15,15));
    putText(dataframe, yks, Point(0,28), FONT_HERSHEY_PLAIN, 1, Scalar(15,15,15));
    imshow("Kalman Data", dataframe);
}

void MomentUtility::filterDensities()
{
    density_t curr, mXt, mYt, mXtv, mYtv;
    mXt = yk.value;
    mYt = xk.value;
    
    int punishf = 5;
    
    for( int x = 0; x < w; x++ )
    {
        curr = dY[x];
        if(curr <= mXt) fY[x] = curr;
        else fY[x] = mXt - punishf*(curr - mXt);
        
        mXtv = mXt - mXv;
        if(fY[x] < mXtv ) fY[x] = 0;
    }
    for( int y = 0; y < h; y++ )
    {
        curr = dX[y];
        if(curr <= mYt) fX[y] = curr;
        else fX[y] = mYt - punishf*(curr - mYt);
        
        mYtv = mYt - mYv;
        if(fX[y] < mYtv ) fX[y] = 0;
    }
}

void MomentUtility::analyzeBlobs()
{
    density_t curr;
    nbX = nbY = 0;
    bool has = false;
    double cavg = 0, mavg = 0;
    int count = 0;
    for( int x = 0; x < w; x++ )
    {
        curr = fY[x];
        if(curr)
        {
            count++;
            if(!has) has = true;
            cma(curr, &cavg, count);
            cma(curr*x, &mavg, count);
        }
        else
        {
            if(has)
            {
                if(nbY < MAX_BLOBS) bY[nbY++] = mavg/cavg;
                mavg = 0;
                cavg = 0;
                count = 0;
                has = false;
            }
        }
    }
    cavg = 0;
    mavg = 0;
    count = 0;
    has = false;
    for( int y = 0; y < h; y++ )
    {
        curr = fX[y];
        if(curr)
        {
            count++;
            if(!has) has = true;
            cma(curr, &cavg, count);
            cma(curr*y, &mavg, count);
        }
        else
        {
            if(has)
            {
                if(nbX < MAX_BLOBS) bX[nbX++] = mavg/cavg;
                cavg = 0;
                count = 0;
                has = false;
            }
        }
    }
}

void MomentUtility::drawDensitiesOnFrame()
{
    drawDensityGraph(frame);
    drawDensityMaps(frame);
}
void MomentUtility::drawDensityGraph(Mat M)
{
    int c, /*p1,*/ p2, u, o, f;
    Vec3b greyish(25,25,25), bluish(255,255,100), greenish(100,255,100), redish(50,100,255), orangish(100,150,255), yellowish(100,255,255), white(255,255,255);
    

    int n = h-(xk.value/DENSITY_SCALE), m = w-(yk.value/DENSITY_SCALE);
//    int n = h-(mX/DENSITY_SCALE), m = w-(mY/DENSITY_SCALE);
    
//    int min_thresh = 10;
//    int nt = h-min_thresh, mt = w-min_thresh;
    
    int nv1 = n-(mXv/DENSITY_SCALE), nv2 = n+(mXv/DENSITY_SCALE), mv1 = m-(mYv/DENSITY_SCALE), mv2 = m+(mYv/DENSITY_SCALE);
    
    line(M, Point(n,0), Point(n,W), orangish);
    line(M, Point(nv1,0), Point(nv1,W), yellowish);
    line(M, Point(nv2,0), Point(nv2,W), yellowish);
    for( int y = 0, p1 = p2 = 0; y < h; y++ )
    {
        u = INR(w-(dX[y]/DENSITY_SCALE),M.rows);
        o = abs(n - u);
        c = INR(n+o,M.rows);
        
        if(c>nv2)M.at<Vec3b>(y,c) = greyish;
//        else M.at<Vec3b>(y,c) = bluish;
//        if(u<n) M.at<Vec3b>(y,u) = redish;
        
        f = w-(fX[y]/DENSITY_SCALE);
        M.at<Vec3b>(y,f) = white;
//        if(c < nt && p1 < c && p1 <= p2) line(M, Point(0,y), Point(w,y), greenish);
        p2 = p1;
        p1 = c;
    }
    
    line(M, Point(0,m), Point(H,m), orangish);
    line(M, Point(0,mv1), Point(H,mv1), yellowish);
    line(M, Point(0,mv2), Point(H,mv2), yellowish);
    for( int x = 0, p1 = p2 = 0; x < w; x++ )
    {
        u = INR(h-(dY[x]/DENSITY_SCALE),M.cols);
        o = abs(m - u);
        c = INR(m+o,M.cols);
        
        if(c>mv2) M.at<Vec3b>(c,x) = greyish;
//        else M.at<Vec3b>(c,x) = bluish;
//        if(u<m) M.at<Vec3b>(u,x) = redish;
        
        int f = h-(fY[x]/DENSITY_SCALE);
        M.at<Vec3b>(f,x) = white;
//        if(c < mt && p1 < c && p1 <= p2) line(M, Point(x,0), Point(x,h), greenish);
        p2 = p1;
        p1 = c;
    }
    
    for( int i = 0; i < nbY; i++ ) line(M, Point(bY[i],0), Point(bY[i],h), greenish);
    for( int i = 0; i < nbX; i++ ) line(M, Point(0,bX[i]), Point(W,bX[i]), greenish);
}

void MomentUtility::drawDensityMaps(Mat M)
{
    Vec3b c(0,0,0);
    for( int y = 0; y < h; y++ )
    {
        int v = dX[y]/mX * 255;
        c = densityColor(v);
        line(M, Point(w,y), Point(W,y), c);
    }
    
    for( int x = 0; x < w; x++ )
    {
        int v = dY[x]/mY * 255;
        c = densityColor(v);
        line(M, Point(x,h), Point(x,H), c);
    }
}

Vec3b MomentUtility::densityColor( int v )
{
    int h, s;
    h = v * 120 / 255;
    s = abs(v - (255/2));
    Vec3b hsv(h,s,200), ret(0,0,0);
    ret = hsv2bgr(hsv);
    return ret;
}

int MomentUtility::pixelDensity( Vec3b p )
{
    return p[2] / 128;
}

Vec3b hsv2bgr(Vec3b hsv)
{
    float h = hsv[0] *   2.0f; // 0-360
    float s = hsv[1] / 255.0f; // 0.0-1.0
    float v = hsv[2] / 255.0f; // 0.0-1.0
    
    float r = 0, g = 0, b = 0; // 0.0-1.0
    
    int   hi = (int)(h / 60.0f) % 6;
    float f  = (h / 60.0f) - hi;
    float p  = v * (1.0f - s);
    float q  = v * (1.0f - s * f);
    float t  = v * (1.0f - s * (1.0f - f));
    
    switch(hi) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    
    return Vec3b(r*255,g*255,b*255);
}
