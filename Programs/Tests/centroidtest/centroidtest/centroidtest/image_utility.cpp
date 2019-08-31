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
VideoCapture cam(0);
#endif

#define ROTATE_IMAGE

image_test::image_test(int argc, const char * argv[], int dimension)
{
    no_file = true;
    Mat image,frame;
    counter = 0;
    num_frames = 1;
    
    subdir = "old";
    if(argc == 2)
    {
        std::string file(IMAGE_ROOT);
        file.append(argv[1]);
        file.append(".bmp");
        printf("opening file: %s\n", file.c_str());
        image = imread(file, IMREAD_COLOR );
        no_file = false;
        if( image.empty() )                      // Check for invalid input
        {
            cout <<  "Could not open or find the image" << std::endl ;
            return;
        }
    }
    else if(argc > 2)
    {
        no_file = false;
        num_frames = atoi(argv[1]);
        subdir = argv[2];
        //printf("Using gif with %d frames.\n",num_frames);
        counter = num_frames;
#ifdef IMAGE_PATH
        std::string file(IMAGE_PATH);
#else
        std::string file(IMAGE_ROOT);
        file.append("frames/");
        file.append(subdir);
        file.append("/1.png");
#endif
        //printf("opening file: %s\n", file.c_str());
        image = imread(file, IMREAD_COLOR );
        if( image.empty() )                      // Check for invalid input
        {
            cout <<  "Could not open or find: " << file << std::endl ;
            return;
        }
    }
    
    if(dimension == 0)
    {
        width = CAM_WIDTH;
        height = CAM_HEIGHT;
        size.width = FNL_RESIZE;
        size.height = FNL_RESIZE;
    }
    else
    {
        width = dimension;
        height = dimension;
        size.width = dimension;
        size.height = dimension;
    }
    
    if(!no_file)
    {
        resize(image,frame,size);
    }
    else
    {
#ifdef HAS_CAMERA
        cam.set(CV_CAP_PROP_FRAME_WIDTH, width);
        cam.set(CV_CAP_PROP_FRAME_HEIGHT, height);
        if (!cam.isOpened()) cout << "cannot open camera" << endl;
        cam.read(frame);
        printf("Init-ing camera: (%d, %d)\n", frame.cols, frame.rows);
#endif
    }
    
    width  = frame.cols;
    height = frame.rows;
    
//    printf("Using frame size %dx%d.\n", width, height);
    
    live = false;
    
    //    if(no_file)printf("Using webcam.\n");
    //    else printf("Using file.\n");
}

void image_test::loop(char c)
{
    if (c == 03) printf("Next frame.\n");
    else if (c == 02)
    {
        counter -= 2;
        if( counter < 0 ) counter += num_frames;
        printf("Last frame. %d\n ", counter);
    }
    else if (c == ' ') live = !live;
}

Mat image_test::getNextFrame()
{
    Mat image, frame(height, width, CV_8UC3, Scalar(0,0,0));
    
    //    printf("\t\tCounter is %d.\n", counter);
    if(counter > 0)
    {
        if(!no_file)
        {
#ifdef IMUT_DEBUG
            printf("\tReading file...\n");
#endif
            counter %= num_frames;
            counter++;
            
#ifdef IMAGE_PATH
            std::string file(IMAGE_PATH);
#else
            std::string file(IMAGE_ROOT);
            file.append("frames/");
            file.append(subdir);
            file.append("/");
#ifdef ROTATE_IMAGE
            file.append(to_string( 1 ));
#else
            file.append(to_string( counter ));
#endif
            file.append(".png");
#endif
            image = imread(file, IMREAD_COLOR );
            if( image.empty() )
            {
                cout <<  "Could not open or find the image " << file << std::endl ;
                return frame;
            }
#ifdef ROTATE_IMAGE
            double angle = 360 * (double)counter / (double)num_frames;
            
            // get rotation matrix for rotating the image around its center in pixel coordinates
            cv::Point2f center((image.cols-1)/2.0, (image.rows-1)/2.0);
            cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
            // determine bounding rectangle, center not relevant
            cv::Rect2f bbox = cv::RotatedRect(cv::Point2f(), image.size(), angle).boundingRect2f();
            // adjust transformation matrix
            rot.at<double>(0,2) += bbox.width/2.0 - image.cols/2.0;
            rot.at<double>(1,2) += bbox.height/2.0 - image.rows/2.0;
            
            cv::Mat dst;
            cv::warpAffine(image, image, rot, bbox.size());
#endif
        }
        else
        {
#ifdef HAS_CAMERA
            cam.read(image);
#endif
        }
        cv::threshold(image, image, 150, 255, 0);
        resize(image,frame,size);
    }
    return frame;
}

int image_test::getWidth()
{
    return width;
}
int image_test::getHeight()
{
    return height;
}
int image_test::getIteration()
{
    return iteration;
}
int image_test::getCounter()
{
    return counter;
}
bool image_test::isLive()
{
    return live;
}
