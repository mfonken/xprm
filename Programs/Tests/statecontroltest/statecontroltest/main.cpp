//
//  main.cpp
//  hmmtest
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "data.h"
#include "psm.h"

using namespace std;
using namespace cv;

#define NUM_OBSERVATIONS 30
#define SAMPLE_POINT_RADIUS 4
#define COV_SCALE 7

Mat gmmMat(HEIGHT, WIDTH, CV_8UC3, Scalar(245,245,245));

double nu = 3;

psm_t psm;

static double GetCovarianceAngle( mat2x2 * covariance )
{
    double a = covariance->a, b = covariance->b, d = covariance->d,
    a_minus_d = a - d,
    radius = sqrt( a_minus_d * a_minus_d + 4. * b * b ),
    lambda = a_minus_d + radius;
    return atan2( -2 * b, a - lambda ) * 180 / M_PI;
}

int main(int argc, const char * argv[])
{
    gmmMat = Scalar(245,245,245);
    SampleList A_samples = GenerateNGaussian2DSamples( A_gaus, NUM_OBSERVATIONS );
    SampleList B_samples = GenerateNGaussian2DSamples( B_gaus, NUM_OBSERVATIONS );
    SampleList C_samples = GenerateNGaussian2DSamples( C_gaus, NUM_OBSERVATIONS );
    PlotSamples(gmmMat, A_samples, SAMPLE_POINT_RADIUS, colors[1]);
    PlotSamples(gmmMat, B_samples, SAMPLE_POINT_RADIUS, colors[2]);
    PlotSamples(gmmMat, C_samples, SAMPLE_POINT_RADIUS, colors[4]);
    ObservationList A_observations = new observation_t[A_samples.size()];
    ObservationList B_observations = new observation_t[B_samples.size()];
    ObservationList C_observations = new observation_t[C_samples.size()];
    ConvertSampleListToObservationList(A_samples, A_observations, A_label);
    ConvertSampleListToObservationList(B_samples, B_observations, B_label);
    ConvertSampleListToObservationList(C_samples, C_observations, C_label);
    
    /*** Run test ***/
    PSMFunctions.Initialize( &psm );
    
    PSMFunctions.ReportObservations( &psm, A_observations, NUM_OBSERVATIONS );
    PSMFunctions.Update( &psm, nu );
    /****************/
    
    delete [] A_observations;
    delete [] B_observations;
    delete [] C_observations;
    return 0;
}
