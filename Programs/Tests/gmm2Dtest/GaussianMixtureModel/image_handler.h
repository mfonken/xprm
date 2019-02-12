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
struct pixel
{
    int x, y, v;
};
vector<pixel> Dxy;

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

vector<pixel>& GenerateDxy(Mat& M)
{
    Mat T;
    M.copyTo(T);
    cv::threshold(M, T, THRESHOLD, BRIGHTNESS, 0);
    int w = M.cols, h = M.rows, s = 0;
    Dxy.clear();

    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            s = T.at<Vec3b>(y,x)[2];
            Dxy.push_back({x,y,s});
        }
    }
    return Dxy;
}


#endif /* image_handler_h */
