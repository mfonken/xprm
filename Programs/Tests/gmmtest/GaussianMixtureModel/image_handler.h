//
//  image_handler.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 11/4/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef image_handler_h
#define image_handler_h

#include <highgui.hpp>
#include <imgproc.hpp>

using namespace cv;
using namespace std;

#define BRIGHTNESS 255
#define THRESHOLD 200
#define SUBSAMPLE_INTERVAL 1

#define ROOT "/Users/matthew/Desktop/PersonalResources/TestImages/"

static Mat image;
vector<int> Dx, Dy;

Mat& LoadImage(string path, int size)
{
    image = imread(ROOT + path, IMREAD_COLOR );
    if(image.cols == 0)
    {
        printf("Image read error.\n");
        getchar();
        return image;
    }
    resize(image,image, Size(size,size), 0, 0, INTER_LINEAR);
    return image;
}

vector<int>& GenerateDx(Mat& M, double * a)
{
    Mat T(M);
    cv::threshold(M, T, THRESHOLD, BRIGHTNESS, 0);
    Dx.clear();
    int w = M.cols, h = M.rows, s = 0;
    for(int y = 0; y < h; y++)
    {
        s = 0;
        for(int x = 0; x < w; x+=SUBSAMPLE_INTERVAL)
        {
            s += T.at<Vec3b>(y,x)[2];
        }
        Dx.push_back(s);
        a[y] = s;
    }
    return Dx;
}


#endif /* image_handler_h */
