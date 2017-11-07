//
//  main.cpp
//  tau+
//
//  Created by Matthew Fonken on 8/23/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include <stdio.h>
#include <stdint.h>

#include "test_setup.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <sys/time.h>

#include "global_types.h"
#include "tau_test.hpp"
#include "statem.h"
#include "tau.h"
#include "rho.h"

#include "image_utility.hpp"

using namespace cv;
using namespace std;

int main( int argc, const char * argv[] )
{
    image_test util(argc, argv);
    
    int width = util.getWidth();
    int height = util.getHeight();

    tau_t tau;
    initTauA(&tau, width, height );
    
    int size = sizeof(prediction_pair_t);
    printf("Size of tau is %dbytes\n", size);

    pixel_base_t **pixels;
    initImg(&pixels, width, height);
    
    int draw = 4;
    
    int threshold = THRESHOLD;
    double t[3];
    
    for(int l=0;l<1;) {
        
        /* Re-init frame and out images every loop */
        Mat frame, out(height, width, CV_8UC3, Scalar(0,0,0));
        frame = util.getNextFrame();
        
        /* Convert image to filtered pixel array and back to black and white image */
        MatToCimage( width, height, frame, pixels, threshold );
        cimageToBandW( width, height, pixels, out );

        /* Run Tau */
        performTauA(&tau, t, pixels);
        
        /* Update threshold */
        int n = stateNumber(tau.sys.state);
        if( n > NUM_OF_SELECTIONS ) threshold += THRESH_STEP;
        if( n < NUM_OF_SELECTIONS ) threshold -= THRESH_STEP;
        if(threshold>THRESHOLD_MAX) threshold = THRESHOLD_MAX;
        if(threshold<THRESHOLD_MIN) threshold = THRESHOLD_MIN;
#ifdef MAIN_DEBUG
        printf("Threshold for state %s is %d. - (%d)\n", stateString(sys.state), threshold, n );
#endif
        
#ifdef STATEM_DEBUG
        printBayesianMap(&tau.sys.probabilities, tau.sys.state);
#endif
        
        /* Draw time division array */
        double a[draw];
        a[0] = 0.0;
        a[1] = t[0];
        a[2] = t[0]+t[1];
        a[3] = a[2]+t[2];
        drawTiming(a, draw);
        
        /* Update FPS display on main output image */
        string text = "FPS:" + to_string(1/a[2]);
        cv::putText(out, text, Point(3, 18), FONT_HERSHEY_SIMPLEX, 0.5, Scalar::all(255), 1, 8);
        
        /* Draw Tau output image */
        drawTau(out, &tau.rho.density_map_pair, &tau.rho.peak_list_pair, &tau.predictions);
        drawDensityMaps(&tau.rho.density_map_pair);
        
        /* Wait for timer or key press */
        util.loop(waitKey(util.isLive()?FRAME_DELAY_MS:60000));
    }
    return 0;
}
