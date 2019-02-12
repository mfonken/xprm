//
//  main.cpp
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 10/27/18.
//  Copyright © 2018 Matthew Fonken. All rights reserved.
//

#include <iostream>
#include "data.h"
#include "gaussianmixture.h"
#include "image_handler.h"

using namespace std;
using namespace cv;

#define INITIAL_VARIANCE    0.1
#define MAX_ERROR           0.1

#define DIMI 2
#define DIMO DIMI

#define ELEMENT_RADIUS 3

#define INC 20

Mat gmmMat(HEIGHT, WIDTH, CV_8UC3, Scalar(245,245,245));

int main(int argc, const char * argv[])
{
    return 0;
}

