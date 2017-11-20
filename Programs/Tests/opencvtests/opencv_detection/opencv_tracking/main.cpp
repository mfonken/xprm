//
//  opencv.cpp
//
//
//  Created by Matthew Fonken on 11/7/17.
//
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/opencv.hpp"

#include <sys/time.h>

#include "image_utility.hpp"

#ifndef SHOW_IMAGES
#define ITERATIONS 10000
#endif

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }

int num_orders = 4;
int orders[] = { 10, 100, 1000, 10000, 100000 };

using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

int main(int argc, const char *argv[])
{
    image_test util(argc, argv);
    Mat frame;
    struct timeval a,b;
    
    for(int o = 0; o < num_orders; o++)
    {
        int iterations = orders[o];
        double times[iterations];
        for(int l=0;l<iterations;l++)
        {
/*****************/
            // Read image
            Mat im = util.getNextFrame();
            bitwise_not ( im, frame );
            
            // Setup SimpleBlobDetector parameters.
            SimpleBlobDetector::Params params;
            
            // Change thresholds
            params.minThreshold = 10;
            params.maxThreshold = 200;
            
            // Filter by Area.
            params.filterByArea = true;
            params.minArea = 1500;
            
            // Filter by Circularity
            params.filterByCircularity = true;
            params.minCircularity = 0.1;
            
            // Filter by Convexity
            params.filterByConvexity = true;
            params.minConvexity = 0.87;
            
            // Filter by Inertia
            params.filterByInertia = true;
            params.minInertiaRatio = 0.01;
            
            
            // Storage for blobs
            vector<KeyPoint> keypoints;
            
            
    #if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2
            
            // Set up detector with params
            SimpleBlobDetector detector(params);
            
            // Detect blobs
            gettimeofday( &a, NULL);
            detector.detect( frame, keypoints);
            gettimeofday( &b, NULL);
            times[l] = timeDiff(a, b);
            
    #else
            
            // Set up detector with params
            Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);
            
            // Detect blobs
            gettimeofday( &a, NULL);
            detector->detect( frame, keypoints);
            gettimeofday( &b, NULL);
            times[l] = timeDiff(a, b);
    #endif
            
            // Draw detected blobs as red circles.
            // DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
            // the size of the circle corresponds to the size of blob
            
            Mat im_with_keypoints;
            drawKeypoints( frame, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
            
            // Show blobs
#ifdef SHOW_IMAGE
            imshow("keypoints", im_with_keypoints );
#endif
            util.loop(waitKey(1));
/*****************/
        }
        long double average = 0;
        for(int l = 0; l < iterations; l++) average += times[l];
        average /= iterations;
        printf("L-%d A-%Lf\n", iterations, average);
    }
}
