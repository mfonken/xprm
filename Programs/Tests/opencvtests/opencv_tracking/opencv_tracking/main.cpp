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

#include <sys/time.h>

#include "image_utility.hpp"

#ifndef SHOW_IMAGES
#define ITERATIONS 1000
#endif

static inline double timeDiff( struct timeval a, struct timeval b ) { return ((b.tv_sec  - a.tv_sec) + (b.tv_usec - a.tv_usec)/1000000.0); }

typedef enum
{
    BOOSTING = 0,
    MIL,
    KCF,
    TLD,
    MEDIANFLOW,
//    GOTURN,
    
    NUM_ALG
} TRACKER;

static inline const char *algorithmString(int s)
{
    static const char *strings[] = { "BOOSTING", "MIL", "KCF", "TLD", "MEDIANFLOW"};//, "GOTURN"};
    return strings[s];
}

int num_orders = 2;
int orders[] = { /*10, 100, */ 1000, 2000, 10000, 100000 };

using namespace cv;
using namespace std;

// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

int main(int argc, const char *argv[])
{
    image_test util(argc, argv);
    
    // List of tracker types in OpenCV 3.2
    // NOTE : GOTURN implementation is buggy and does not work.
    string trackerTypes[] =
    {
        "BOOSTING",
        "MIL",
        "KCF",
        "TLD",
        "MEDIANFLOW",
//        "GOTURN"
    };
    // vector <string> trackerTypes(types, std::end(types));
    for(int o = 0; o < num_orders; o++)
    {
        for(int i = 0; i < NUM_ALG; i++)
        {
            // Create a tracker
            string trackerType = trackerTypes[i];
            
            Ptr<Tracker> tracker;
            
            //    #if (CV_MINOR_VERSION < 3)
            //        {
            //            tracker = Tracker::create(trackerType);
            //        }
            //    #else
            {
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
            }
            //    #endif
            
            // Read first frame
            Mat frame;
            frame = util.getNextFrame();
            // Define initial boundibg box
            Rect2d bbox(FNL_RESIZE/4, FNL_RESIZE/2, 50, 50);
            
            // Uncomment the line below to select a different bounding box
            //    bbox = selectROI(frame, false);
            
            // Display bounding box.
            rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
            //        imshow("Tracking", frame);
            
            tracker->init(frame, bbox);
            struct timeval a,b,c,d;
            
            int iterations = orders[o];
            double times[iterations];
            printf("%s: ", algorithmString(i));
            
            gettimeofday( &a, NULL);
            for(int l=0;l<iterations;l++)
            {
                bitwise_not ( util.getNextFrame(), frame );
                // Update the tracking result
                
                gettimeofday( &c, NULL);
                tracker->update(frame, bbox);
                gettimeofday( &d, NULL);
                times[l] = timeDiff(c, d);
                
                printf(" Size: %luB\n", sizeof(*tracker));
                
#ifdef SHOW_IMAGES
                // Start timer
                double timer = (double)getTickCount();
                
                // Update the tracking result
                bool ok = tracker->update(frame, bbox);
                
                // Calculate Frames per second (FPS)
                float fps = getTickFrequency() / ((double)getTickCount() - timer);
                
                if (ok)
                {
                    // Tracking success : Draw the tracked object
                    rectangle(frame, bbox, Scalar( 255, 0, 0 ), 2, 1 );
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
                imshow("Tracking", frame);
#endif
                
                // Exit if ESC pressed.
                util.loop(waitKey(1));
            }
            
            gettimeofday( &b, NULL);
            long double average = 0;
            for(int l = 0; l < iterations; l++) average += times[l];
            average /= iterations;
            
            double total_time = timeDiff(a, b);
            printf("total time: %lfs | iterations: %d | average time: %Lfms\n", total_time, iterations, average * 1000);
        }
    }
}

