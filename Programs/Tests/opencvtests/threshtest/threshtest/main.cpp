//
//  main.cpp
//  threshtest
//
//  Created by Matthew Fonken on 5/4/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

#define TITLE_STRING                    "Thresh Test Xprm"

#define FRAME_IMAGE_ROOT                "/Users/matthew/Desktop/PersonalResources/TestImages/"
#define GROUP_NAME                      "gradient"//"frames"
#define FILE_NAME                       "gradient_centralized"
    //"gradient_centralized"//"double_circle_corner_triangle_fade_high_contrast"//"triple_circle_corner_triangle_fade_high_contrast"// noiseS1"//"double_circle_corner_triangle_fade"//"single_triangle_fade"//"double_circle_fade"//"single_circle_fade"
#define FILE_TYPE                       ".png"

#define FRAME_IMAGE_IMAGE_SOURCE_PATH   FRAME_IMAGE_ROOT GROUP_NAME "/" FILE_NAME FILE_TYPE

int threshold_value = 0;
double threshold_scale = 10;
int threshold_type = 3;
int const max_value = 255/(int)threshold_scale;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, src_gray, dst;
const char* window_name = "Threshold Demo";

const char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
const char* trackbar_value = "Value";

/// Function headers
void Threshold_Demo( int, void* );

/**
 * @function main
 */
int main( int argc, char** argv )
{
    printf("Loading: %s\n", FRAME_IMAGE_IMAGE_SOURCE_PATH);
    /// Load an image
    src = imread( FRAME_IMAGE_IMAGE_SOURCE_PATH, 1 );
    
    /// Convert the image to Gray
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    
    /// Create a window to display results
    namedWindow( window_name, WINDOW_AUTOSIZE );
    
    /// Create Trackbar to choose type of Threshold
    createTrackbar( trackbar_type,
                   window_name, &threshold_type,
                   max_type, Threshold_Demo );
    
    createTrackbar( trackbar_value,
                   window_name, &threshold_value,
                   max_value, Threshold_Demo );
    
    /// Call the function to initialize
    Threshold_Demo( 0, 0 );
    
    /// Wait until user finishes program
    while(true)
    {
        int c;
        c = waitKey( 20 );
        if( (char)c == 27 )
        { break; }
    }
    
}

template <typename T>
static std::string to_stringn(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}

double CalculatePercentActive( const Mat &M )
{
    int nRows = M.rows,
        nCols = M.cols,
        nChan = M.channels(),
        count = 0,
        total = nRows * nCols;
    uchar* p = M.data;
    for( unsigned int i = 0; i < nCols * nRows * nChan; i++)
        if( *p++ > 0 ) count++;
    
    return (double)count/(double)total;
}

void Threshold_Demo( int, void* )
{
    /* 0: Binary
     1: Binary Inverted
     2: Threshold Truncated
     3: Threshold to Zero
     4: Threshold to Zero Inverted
     */
    
    int scaled_thresh = (int)(threshold_value*threshold_scale);
    threshold( src_gray, dst, scaled_thresh, max_BINARY_value, threshold_type );
    
    double percentActive = CalculatePercentActive(dst);
    putText(dst, to_stringn(percentActive*100,2) + "%", Point(0, 34*2), FONT_HERSHEY_PLAIN, 3, Scalar(255), 2);
    putText(dst, to_string(scaled_thresh), Point(0, 34), FONT_HERSHEY_PLAIN, 3, Scalar(255), 2);
    imshow( window_name, dst );
}
