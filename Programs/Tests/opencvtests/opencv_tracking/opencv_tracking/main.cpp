//
//  opencv.cpp
//
//
//  Created by Matthew Fonken on 11/7/17.
//
//

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/ocl.hpp>
#include <opencv2/tracking.hpp>
#include <fstream>

#include "centroid.h"
#include <sys/time.h>

#include "image_utility.hpp"

#define DEG_TO_RAD(X)   (X / 180. * M_PI)
#define BASE_ANGLE      41
#define BASE_A_d_F      0.070
#define BASE_A_X_F      0.730
#define BASE_A_Y_F      0.286
#define BASE_A_R_F      0.314159

#define MAX_DIFF_PERC   0.20

#define FRAME_NAME "Tracking"
#define FILE_NAME "/Users/matthew/Desktop/opencv_tracking.csv"
#define DATA_FILE_NAME "/Users/matthew/Desktop/opencv_tracking_data.csv"

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }

int num_orders = 1;
int orders[] = { 200, 2000, 10000, 100000 };

int dimensions[] = { 100, 250 };//500, 750, 1000, 1250, 1500, 1750, 2000 }; //2000, 1750, 1500, 1250, 1000, 750, 500, 250, 100 };
int num_dimensions = sizeof(dimensions)/ sizeof(dimensions[0]);

using namespace cv;
using namespace std;

ofstream file;

#ifdef CAMERA_ID
VideoCapture cam(CAMERA_ID);
#endif

void InitCamera()
{
    cam.set(CAP_PROP_FRAME_WIDTH,  CAM_WIDTH);
    cam.set(CAP_PROP_FRAME_HEIGHT, CAM_HEIGHT);
    cam.set(CAP_PROP_FPS,          CAM_FRAME_RATE);
    
    if (!cam.isOpened())
    {
        printf("Could not open or find camera.\n");
        while(1);
        return;
    }
    
    printf("Initializing Camera: %dx%d @ %d fps.\n", (int)cam.get(CAP_PROP_FRAME_WIDTH), (int)cam.get(CAP_PROP_FRAME_HEIGHT), (int)cam.get(CAP_PROP_FPS));
}

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

// Fill the vector with random colors
void getRandomColors(vector<Scalar>& colors, int numColors)
{
    RNG rng(0);
    for(int i=0; i < numColors; i++)
        colors.push_back(Scalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255)));
}

void run(int argc, const char *argv[], int dimension)
{
    // List of tracker types in OpenCV 3.2
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[] =
    {
        "MEDIANFLOW",
//        "CENTROIDS",
//        "CSRT",
//        "BOOSTING",
//        "MIL",
//        "KCF",
        //"TLD",
//        "MOSSE"
    };
    
    Point start_point( (BASE_A_R_F * cos(DEG_TO_RAD(-BASE_ANGLE)) + 0.5 - BASE_A_d_F/2.) * (double)dimension, (BASE_A_R_F * sin(DEG_TO_RAD(-BASE_ANGLE)) + 0.5 - BASE_A_d_F/2.) * (double)dimension);
    Size start_size(BASE_A_d_F*(double)dimension, BASE_A_d_F*(double)dimension);
    Rect start_rect( start_point, start_size);
    
    // vector <string> trackerTypes(types, std::end(types));
    for(int o = 0; o < num_orders; o++)
    {
        for(int i = 0; i < sizeof(trackerTypes)/sizeof(trackerTypes[0]); i++)
        {
            image_test util(argc, argv, dimension);
            Rect A((double)dimension/2., (double)dimension/2.,1,1);
            // Read first frame
            Mat frame;
#ifdef CAMERA_ID
            cam >> frame;
#else
            frame = util.getNextFrame();
#endif
            vector<Scalar> colors;
            
            // Create a tracker
            string trackerType = trackerTypes[i];
            Ptr<MultiTracker> multiTracker;
            Ptr<Tracker> tracker;
            if (trackerType == "BOOSTING")
                tracker = TrackerBoosting::create();
            if (trackerType == "MIL")
                tracker = TrackerMIL::create();
            if (trackerType == "KCF")
                tracker = TrackerKCF::create();
            if (trackerType == "TLD")
                tracker = TrackerTLD::create();
            if (trackerType == "MEDIANFLOW")
                tracker = TrackerMedianFlow::create();
            if (trackerType == "GOTURN")
                tracker = TrackerGOTURN::create();
            if (trackerType == "MOSSE")
                tracker = TrackerMOSSE::create();
            if (trackerType == "CSRT")
                tracker = TrackerCSRT::create();
            if(trackerType != "CENTROIDS")
            {
                // Uncomment the line below to select a different bounding box
                vector<Rect> bboxes;
                bboxes.push_back(start_rect);
                //            bool showCrosshair = false;
                //            bool fromCenter = false;
                //            selectROIs(FRAME_NAME, frame, bboxes, showCrosshair, fromCenter);
                
                // quit if there are no objects to track
                if(bboxes.size() < 1)
                    continue;
                
                getRandomColors(colors, (int)bboxes.size());
                
                // Create multitracker
                multiTracker = cv::MultiTracker::create();
                
                // Initialize multitracker
                for(int i=0; i < bboxes.size(); i++)
                {
                    // printf("%d: (%d, %d)>%dx%d\n", i, bboxes[i].x, bboxes[i].y, bboxes[i].width, bboxes[i].height);
                    multiTracker->add(tracker, frame, Rect2d(bboxes[i]));
                }
            }
            struct timeval a,b,c,d;
            
            int iterations = orders[o];
            double times[iterations];
            double diffs[iterations];
            
            bool ok = false;
            
            gettimeofday( &a, NULL);
            for(int l=0;l<iterations;l++)
            {
                frame = util.getNextFrame();
                centroids_test_t ct;
                double angle = 2. * M_PI * -(double)util.counter / (double)util.num_frames - DEG_TO_RAD(BASE_ANGLE);
                Point expected( (BASE_A_R_F * cos(angle) + 0.5 - BASE_A_d_F/2.) * (double)dimension, (BASE_A_R_F * sin(angle) + 0.5 - BASE_A_d_F/2.) * (double)dimension);
                Point expected_end(expected.x + BASE_A_d_F*(double)dimension, expected.y + BASE_A_d_F*(double)dimension);
                
                //                printf("x:%d y:%d t:%f\n", expected.x, expected.y, diffs[l]);
                if(trackerType == "CENTROIDS")
                {
                    initCentroids(&ct, frame.cols, frame.rows);
                    char line[2000] = { 0 };
                    gettimeofday( &c, NULL);
                    int y = 0;
                    for(; y < frame.rows; y++)
                    {
                        char * line_p = line;
                        for(int x = 0; x < frame.cols; x++)
                        {
                            *(line_p++) = (char)frame.at<Vec3b>(y,x)[2];
                        }
                        getCentroids(&ct, (uint8_t *)line, y, 1);
                    }
                    gettimeofday( &d, NULL);
                    times[l] = timeDiff(c, d);
                    ok = ct.centroids.numBlobs > 0;
                    
#ifdef SHOW_IMAGES
                    for(int i = 0; i < ct.centroids.numBlobs; i++ )
                    {
                        blob_t * bl = &ct.centroids.blobs[i];
                        int X = bl->X;
                        int Y = bl->Y;
                        if( (X > 0 && X < frame.cols) && (Y > 0 && Y < frame.rows) )
                        {
                            Vec3b R(0,0,255);
                            frame.at<Vec3b>(Y,X-1) = R;
                            frame.at<Vec3b>(Y,X+1) = R;
                            frame.at<Vec3b>(Y,X) = R;
                            frame.at<Vec3b>(Y-1,X) = R;
                            frame.at<Vec3b>(Y-1,X+1) = R;
                        }
                    }
#endif
                }
                else
                {
                    gettimeofday( &c, NULL);
                    //                bool ok = tracker->update(frame, bbox);
                    ok = multiTracker->update(frame);
                    gettimeofday( &d, NULL);
                }
                times[l] = timeDiff(c, d);
#ifdef SHOW_IMAGES
                if(waitKey(1) == ' ')
                {
                    while(waitKey(1) != ' ');
                    while(waitKey(100) == ' ');
                }
                
                
                // Calculate Frames per second (FPS)
                float fps = 1 / times[l];
                
                
                if (ok)
                {
                    if(trackerType != "CENTROIDS")
                    {
                        // Draw tracked objects
                        auto objects = multiTracker->getObjects();
                        for(unsigned i=0; i<objects.size(); i++)
                        {
                            auto object = objects[i];
                            rectangle(frame, object, colors[i], 2, 1);
                        }
                    }
                }
                else
                {
                    // Tracking failure detected.
                    putText(frame, "Tracking failure detected", Point(100,80), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(0,0,255),2);
                }
                // Display tracker type on frame
                putText(frame, trackerType + " Tracker", Point(100,20), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50),2);
                // Display FPS on frame
                putText(frame, "FPS : " + to_string(int(fps)), Point(100,50), FONT_HERSHEY_SIMPLEX, 0.75, Scalar(50,170,50), 2);
                
                // Display frame.
                
#endif
                if(trackerType == "CENTROIDS")
                {
                    int Ax = ct.centroids.blobs[0].X, Ay = ct.centroids.blobs[0].Y;
                    A = Rect(Point(Ax - (BASE_A_d_F/2. * (double)dimension), Ay - (BASE_A_d_F/2. * (double)dimension)), start_size);
                }
                else
                {
                    A = multiTracker->getObjects()[0];
                }
                
                double x_diff = fabs(expected.x - A.x);
                double y_diff = fabs(expected.y - A.y);
                if (!ok)
                {
                    diffs[l] = -1;
                }
                else
                {
                    diffs[l] = sqrt( x_diff * x_diff + y_diff * y_diff );
                    if( trackerType == "CENTROIDS" && diffs[l] > MAX_DIFF_PERC * (double)dimension)
                    {
                        int Ax = ct.centroids.blobs[1].X, Ay = ct.centroids.blobs[1].Y;
                        A = Rect(Point(Ax - (BASE_A_d_F/2. * (double)dimension), Ay - (BASE_A_d_F/2. * (double)dimension)), start_size);
                        double x_diff = fabs(expected.x - A.x);
                        double y_diff = fabs(expected.y - A.y);
                        diffs[l] = sqrt( x_diff * x_diff + y_diff * y_diff );
                    }
                }
#ifdef SHOW_IMAGES
                rectangle(frame, A, Vec3b{0,0,255}, 2, 1);
                imshow(FRAME_NAME, frame);
#endif
                util.loop(waitKey(3));
            }
            
            gettimeofday( &b, NULL);
            long double time_average = 0, diff_average = 0;
            int diff_num = 0;
            for(int l = 0; l < iterations; l++)
            {
                time_average += times[l];
                if(diffs[l] >= 0)
                {
                    diff_average += diffs[l];
                    diff_num++;
                }
            }
            time_average /= iterations;
            diff_average /= (double)diff_num;
            
            long double diff_stddev = 0, v;
            for(int l = 0; l < iterations; l++)
            {
                if(diffs[l] < 0) continue;
                v = diffs[l] - diff_average;
                diff_stddev += v * v;
            }
            diff_stddev /= ( (double)diff_num - 1. );
            diff_stddev = sqrt(diff_stddev);
            
            double total_time = timeDiff(a, b);
            
            file.open(FILE_NAME, ofstream::app);
            char buf[128];
            sprintf(buf, "%s,%d,%d,%lf,%Lf,%Lf,%Lf\n", trackerTypes[i].c_str(), dimension, iterations, total_time, time_average * 1000., diff_average / (double)dimension * 100., diff_stddev / (double)dimension * 100.);
            printf("%s", buf);
            file << string(buf);
            file.close();
        }
        
        file.close();
    }
}

int main(int argc, const char *argv[])
{
    file.open(FILE_NAME);
    file << "Algorithm,Dimension(px),Iterations,Total Time(s),Avg. Time(ms), Avg. Diff.(%), Std. Dev. Diff. (%)\n";
    file.close();
    
#ifdef CAMERA_ID
    InitCamera();
#endif
    
    for(uint8_t i = 0; i < num_dimensions; i++)
    {
        run(argc, argv, dimensions[i]);
    }
    return 0;
}

