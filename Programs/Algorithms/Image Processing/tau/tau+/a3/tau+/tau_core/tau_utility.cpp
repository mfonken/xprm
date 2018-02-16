//
//  tau_utility.cpp
//  tau+
//
//  Created by Matthew Fonken on 2/7/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include "tau_utility.hpp"
#include "utility_functions.hpp"

using namespace cv;

void Tau::init( void )
{
    thresh = THRESHOLD_MIN;
    count = 0;
    avg = 0;
}
void Tau::trigger( void )
{
    Mat frame = utility->outframe;
    image = utility->outimage;
    double p = perform( &image );
    if(count < MAX_COUNT)
        cma(p, &avg, ++count);
}

std::string Tau::serialize( void )
{
    return "";
}

Tau::Tau( std::string name, ImageUtility * util, int width, int height ) : rho(width, height), sigma(width, height)
{
    this->name = name;
    this->utility = util;
    this->width = width;
    this->height = height;
    cimageInit(&image, width, height);
}

double Tau::perform( Mat M )
{
    struct timeval a,b;
    gettimeofday( &a, NULL);
    
    rho.perform( M, &predictions );
    gettimeofday( &b, NULL);
    
//    updatePredictionFromPeaks();
    
//    sigma.perform( &rho.peak_list_pair, &predictions );
//    gettimeofday( &c, NULL);
    
//    sys.update( &predictions );
//    gettimeofday( &d, NULL);
    
//    updatePrediction();
    
    return timeDiff(a,b);
    //    times[1] = timeDiff(b,c);
    //    times[2] = timeDiff(c,d);
}

double Tau::perform( cimage_t * img )
{
    struct timeval a,b;
    gettimeofday( &a, NULL);
    rho.perform( img, &predictions );
    gettimeofday( &b, NULL);
    return timeDiff(a,b);
}

void Tau::updatePrediction()
{
    Point2f a((double)predictions.x.primary,   (double)predictions.y.primary),
            b((double)predictions.x.secondary, (double)predictions.y.secondary);
//    invfisheye(&a, FNL_RESIZE_W, FNL_RESIZE_H, STRENGTH, ZOOM);
//    invfisheye(&b, FNL_RESIZE_W, FNL_RESIZE_H, STRENGTH, ZOOM);
    A = { a.y, a.x };
    B = { b.y, b.x };
    rho.peak_list_pair.x.map[0] = a.x;
    rho.peak_list_pair.x.map[1] = b.x;
    rho.peak_list_pair.x.length = 2;
    rho.peak_list_pair.y.map[0] = a.y;
    rho.peak_list_pair.y.map[2] = b.y;
    rho.peak_list_pair.y.length = 2;
    
    putText(utility->outframe, "A", Point(A.x, A.y), FONT_HERSHEY_PLAIN, 2, Vec3b(0,55,255), 3);
    putText(utility->outframe, "B", Point(B.x, B.y), FONT_HERSHEY_PLAIN, 2, Vec3b(0,255,55), 3);
}

void Tau::updatePredictionFromPeaks()
{
    predictions.x.primary = rho.peak_list_pair.x.map[0];
    predictions.y.primary = rho.peak_list_pair.y.map[0];
    predictions.x.secondary = rho.peak_list_pair.x.map[1];
    predictions.y.secondary = rho.peak_list_pair.y.map[1];
    
    Point2f a((double)predictions.x.primary,   (double)predictions.y.primary),
            b((double)predictions.x.secondary, (double)predictions.y.secondary);
    
    putText(utility->outframe, "A", Point(a.y, a.x), FONT_HERSHEY_PLAIN, 2, Vec3b(0,55,255), 3);
    putText(utility->outframe, "B", Point(b.y, b.x), FONT_HERSHEY_PLAIN, 2, Vec3b(0,255,55), 3);
}
