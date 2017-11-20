//
//  opencv.cpp
//
//
//  Created by Matthew Fonken on 11/7/17.
//
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <sys/time.h>
#include <stdlib.h>
#include "centroid.h"

#include "image_utility.hpp"

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
    Mat frame = util.getNextFrame();
    
    centroids_test_t c;
    
    struct timeval a,b;
    
    initCentroids(&c, frame.cols, frame.rows);
    
    for(int o = 0; o < num_orders; o++)
    {
        int iterations = orders[o];
        double times[iterations];
        for(int l=0;l<iterations;l++)
        {
            frame = util.getNextFrame();
/*****************/
            gettimeofday( &a, NULL);
            for(int y = 0; y < frame.rows; y++)
            {
                getCentroids(&c, (uint8_t *)line, y, 1);
            }
            gettimeofday( &b, NULL);
            times[l] = timeDiff(a, b);
            util.loop(waitKey(1));
/*****************/
        }
        long double average = 0;
        for(int l = 0; l < iterations; l++) average += times[l];
        average /= iterations;
        printf("L-%d A-%Lf\n", iterations, average);
    }
}
