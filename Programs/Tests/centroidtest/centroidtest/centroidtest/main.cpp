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
#include <fstream>
#include "centroid.h"

#include "image_utility.hpp"
#define FILE_NAME "/Users/matthew/Desktop/opencv_tracking.csv"

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }

#define DEG_TO_RAD(X)   (X / 180. * M_PI)
#define BASE_ANGLE      41
#define BASE_A_d_F      0.070
#define BASE_A_X_F      0.730
#define BASE_A_Y_F      0.286
#define BASE_A_R_F      0.314159

int num_orders = 1;
int orders[] = { /*10, */ 100 /*, 1000, 2000, 10000, 100000 */ };

int dimensions[] = { 50, 250, 500, 750, 1000, 1250, 1500, 1750, 2000 };
int num_dimensions = sizeof(dimensions)/ sizeof(dimensions[0]);

using namespace cv;
using namespace std;

ofstream file;

Vec3b R(0,0,255);

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()


void run(int argc, const char *argv[], int dimension)
{
    image_test util(argc, argv, dimension);
    Rect A((double)dimension/2., (double)dimension/2.,1,1);
    Size start_size(BASE_A_d_F*(double)dimension, BASE_A_d_F*(double)dimension);
    for(int o = 0; o < num_orders; o++)
    {
        Mat frame = util.getNextFrame();
        
        centroids_test_t ct;
        
        struct timeval a,b,c,d;
        
        initCentroids(&ct, frame.cols, frame.rows);
        
        int iterations = orders[o];
        double times[iterations];
        double diffs[iterations];
        gettimeofday( &a, NULL);
        for(int l=0;l<iterations;l++)
        {
            frame = util.getNextFrame();
            
            double angle = 2. * M_PI * -(double)util.counter / (double)util.num_frames - DEG_TO_RAD(BASE_ANGLE);
            Point expected( (BASE_A_R_F * cos(angle) + 0.5 - BASE_A_d_F/2.) * (double)dimension, (BASE_A_R_F * sin(angle) + 0.5 - BASE_A_d_F/2.) * (double)dimension);
            Point expected_end(expected.x + BASE_A_d_F*(double)dimension, expected.y + BASE_A_d_F*(double)dimension);
            /*****************/
            
            char line[2000] = { 0 };
//            int y = 0;
//            for(; y < frame.rows; y++)
//            {
//                char * line_p = line;
//                for(int x = 0; x < frame.cols; x++)
//                {
//                    *(line_p++) = frame.at<Vec3b>(x,y)[2];
//                }
//            }
            gettimeofday( &c, NULL);
            int y = 0;
            for(; y < frame.rows; y++)
            {
                char * line_p = line;
                for(int x = 0; x < frame.cols; x++)
                {
                    *(line_p++) = frame.at<Vec3b>(x,y)[2];
                }
                getCentroids(&ct, (uint8_t *)line, y, 1);
            }
            gettimeofday( &d, NULL);
            times[l] = timeDiff(c, d);
            
            for(int i = 0; i < ct.centroids.numBlobs; i++ )
            {
                blob_t * bl = &ct.centroids.blobs[i];
                int X = bl->X;
                int Y = bl->Y;
                if( (X > 0 && X < frame.cols) && (Y > 0 && Y < frame.rows) )
                {
                    frame.at<Vec3b>(Y,X-1) = R;
                    frame.at<Vec3b>(Y,X+1) = R;
                    frame.at<Vec3b>(Y,X) = R;
                    frame.at<Vec3b>(Y-1,X) = R;
                    frame.at<Vec3b>(Y+1,X) = R;
                }
            }
            
            int Ax = ct.centroids.blobs[0].X, Ay = ct.centroids.blobs[0].Y;
            A = Rect(Point(Ax - (BASE_A_d_F/2. * (double)dimension), Ay - (BASE_A_d_F/2. * (double)dimension)), start_size);
            
            rectangle(frame, A, Vec3b{0,0,255}, 2, 1);
            
            double x_diff = fabs(expected.x - A.x);
            double y_diff = fabs(expected.y - A.y);
            
            diffs[l] = sqrt( x_diff * x_diff + y_diff * y_diff );
#ifdef SHOW_IMAGES
            imshow("Frame", frame);
            util.loop(waitKey(1));
#endif
            /*****************/
        }
        gettimeofday( &b, NULL);
        long double time_average = 0, diff_average = 0;
        for(int l = 0; l < iterations; l++)
        {
            time_average += times[l];
            diff_average += diffs[l];
        }
        time_average /= iterations;
        diff_average /= iterations;
        
        long double diff_stddev = 0, v;
        for(int l = 0; l < iterations; l++)
        {
            v = diffs[l] - diff_average;
            diff_stddev += v * v;
        }
        diff_stddev /= ( iterations - 1 );
        diff_stddev = sqrt(diff_stddev);
        
        double total_time = timeDiff(a, b);
        file.open(FILE_NAME, ofstream::app);
        char buf[128];
        sprintf(buf, "CENTROIDS,%d,%d,%lf,%Lf,%Lf,%Lf\n", dimension, iterations, total_time, time_average * 1000., diff_average / (double)dimension * 100., diff_stddev / (double)dimension * 100.);
        printf("%s\n", buf);
        file << string(buf);
        file.close();
    }
}

int main(int argc, const char *argv[])
{
    file.open(FILE_NAME);
    file << "Algorithm,Dimension(px),Iterations,Total Time(s),Avg. Time(ms), Avg. Diff.(%), Std. Dev. Diff. (%)\n";
    file.close();
    for(uint8_t i = 0; i < num_dimensions; i++)
    {
        run(argc, argv, dimensions[i]);
    }
    return 0;
}
