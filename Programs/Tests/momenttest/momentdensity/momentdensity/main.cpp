//
//  main.cpp
//  momentdensity
//
//  Created by Matthew Fonken on 2/3/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#include <iostream>
#include "image_utility.hpp"
#include "moment_utility.hpp"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define FNL_WIDTH  400//640
#define FNL_HEIGHT 400//400

#define FPS         10
#define FRAME_DELAY 1000/FPS

using namespace cv;

int main(int argc, const char * argv[]) {

    ImageUtility iutil(argc, argv, FNL_WIDTH, FNL_HEIGHT);
    
    MomentUtility mutil(iutil.frame);
    
    while(1)
    {
        
        mutil.updateDensities(iutil.getNextFrame());
        
        imshow("Frame", mutil.frame);
        
        iutil.loop(waitKey(FRAME_DELAY));
    }
    
    return 0;
}
