//
//  moment_utility.hpp
//  momentdensity
//
//  Created by Matthew Fonken on 2/3/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef moment_utility_hpp
#define moment_utility_hpp

#include <stdio.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "kalman.hpp"

#define SIDEBAR_WIDTH 100

#define DENSITY_SCALE 1

#define MAX_BLOBS 10

typedef double density_t;

class MomentUtility
{
    density_t *dX, *dY, *fX, *fY;
    density_t mX, mY, mXp, mYp, mXv, mYv;
    
    density_t *bX, *bY;
    int nbX, nbY;
    
    Kalman xk, yk;
    int w, h, W, H;
public:
    cv::Size size;
    cv::Mat image,frame;
    
    MomentUtility(cv::Mat);
    void updateFrame(cv::Mat);
    
    void updateDensities(cv::Mat);
    void updateDensityKalmans();
    void filterDensities();
    void analyzeBlobs();
    void getMaxDensities();
    void getDensities();
    void drawDensitiesOnFrame();
    void drawDensityGraph(cv::Mat);
    void drawDensityMaps(cv::Mat);
    
    cv::Vec3b densityColor( int );
    int pixelDensity( cv::Vec3b );
};

cv::Vec3b hsv2bgr(cv::Vec3b hsv);

#endif /* moment_utility_hpp */
