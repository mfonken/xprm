//
//  image_handler.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 11/4/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#ifndef image_handler_h
#define image_handler_h

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define ROOT "/Users/matthew/Desktop/CSResources/Test Images/"

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
    resize(image,image, Size(size,size), 0, 0, CV_INTER_LINEAR);
    return image;
}

vector<int>& GenerateDx(Mat& M, double * a)
{
    Dx.clear();
    int w = M.cols, h = M.rows, s = 0;
    for(int y = 0; y < h; y++)
    {
        s = 0;
        for(int x = 0; x < w; x++)
        {
            s += M.at<Vec3b>(y,x)[2];
        }
        Dx.push_back(s);
//        a[y] = s;
    }
    return Dx;
}


#endif /* image_handler_h */
