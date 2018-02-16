//
//  image_utilities.cpp
//  tau+
//
//  Created by Matthew Fonken on 10/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "image_utility.hpp"

using namespace cv;
using namespace std;

#ifdef HAS_CAMERA
VideoCapture cam(1);
#endif

//Vec3b preset(BRIGHTNESS,BRIGHTNESS,BRIGHTNESS);

ImageUtility::ImageUtility( int argc, const char * argv[], int width, int height )
{
    Mat temp(height, width, CV_8UC3, Scalar(0,0,0));
    frame = temp;
    size.width = width;
    size.height = height;
    counter = 0;
    num_frames = 1;
    file = IMAGE_ROOT;
    

    if(argc >= 2)
    {
        file.append(argv[1]);
        file.append("/");
        subdir = file;
        if(argc > 2)
        {
            num_frames = atoi(argv[2]);
            printf("Running %d frames.\n", num_frames);
            counter = 1;
            file.append( to_string(counter) );
        }
        file.append(".png");
        printf("opening file: %s\n", file.c_str());
        image = imread(file, IMREAD_COLOR );
        if( image.empty() )                      // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return;
        }
    }
    
    resize(image,frame,size);
    
    width  = frame.rows;
    height = frame.cols;
    
    printf("Using frame size %dx%d.\n", width, height);
    
    live = true;
}

void ImageUtility::loop(char c)
{
    if (c == ' ')
    {
        live = !live;
        getFrame();
        return;
    }
    if (c == 03)
    {
        counter %= num_frames;
        counter++;
//        printf("Next frame.\n");
    }
    else if (c == 02)
    {
        counter--;
        if( counter < 1 ) counter += num_frames;
        
//        printf("Last frame. %d\n ", counter);
    }
    else return;
    getFrame();
}

Mat ImageUtility::getNextFrame()
{
    if(!live) return(frame);
    
    if(counter > 0)
    {
        counter %= num_frames;
        counter++;
    }
    return getFrame();
}

Mat ImageUtility::getFrame()
{
    file = subdir + to_string( counter ) + ".png";
//    cout <<  "Opening " << file << std::endl;
    image = imread(file, IMREAD_COLOR );
    if( image.empty() )
    {
        cout <<  "Could not open or find the image " << file << std::endl ;
        return frame;
    }

    resize(image,frame,size);
    
    return frame;
}
