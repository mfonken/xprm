//
//  test_setup.h
//  tau+
//
//  Created by Matthew Fonken on 10/28/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifndef test_setup_h
#define test_setup_h

#define TITLE_STRING   "OpenCV Tracking Algorithms Test"

/* DEBUGGING */
//#define MAIN_DEBUG
//#define SHOW_IMAGES

/* IMAGE PROCESSING */
#define IMAGE_ROOT "/Users/matthew/Desktop/PersonalResources/TestImages"
//#define IMAGE_GROUP "/gradient"
//#define IMAGE_NAME "/gradient_centralized.png"
#define IMAGE_GROUP "/frames/small"
#define IMAGE_NAME "/1.png"
#define IMAGE_PATH IMAGE_ROOT IMAGE_GROUP IMAGE_NAME
#define CAM_WIDTH   640
#define CAM_HEIGHT  320

#define THRESHOLD   150
#define BRIGHTNESS  100

/* TAU SETUP */
#define DRAW_HEIGHT 300
#define MAP_INSET   80
#define MAP_SCALE   13
#define FNL_RESIZE  1000

/* RHO SETUP */
//#define DEFAULT_SIGMA 3.0 //GAUSSIAN SIGMA

#endif /* test_setup_h */
