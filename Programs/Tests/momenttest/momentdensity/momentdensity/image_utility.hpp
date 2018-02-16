//
//  image_utilities.hpp
//  tau+
//
//  Created by Matthew Fonken on 10/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef image_utilities_hpp
#define image_utilities_hpp

#include <stdio.h>
#include <stdint.h>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/time.h>

#define IMAGE_ROOT "/Users/matthewfonken/Desktop/CSResources/Test Images/"

class ImageUtility
{
private:
    int width;
    int height;
    int iteration;
    int counter;
    int args;
    
    int num_frames;
    
    std::string subdir;
    std::string file;
    
    cv::Size size;

public:
    bool live;
    cv::Mat image, frame;
    
    ImageUtility( int argc, const char * argv[] );
    ImageUtility( int argc, const char * argv[], int width, int height );
    void loop(char c);
    cv::Mat getNextFrame();
    cv::Mat getFrame();
};



#endif /* image_utilities_hpp */
