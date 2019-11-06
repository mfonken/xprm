//
//  test_config.h
//  kineticerrortest
//
//  Created by Matthew Fonken on 11/2/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef test_config_h
#define test_config_h

#define __KALMAN__

#define GLOBAL_COVAR 5.0
#define P_COVAR GLOBAL_COVAR
#define O_COVAR GLOBAL_COVAR

#define OUT_FILE_PATH   "/Users/matthew/Desktop/"
#define OUT_FILE_NAME   OUT_FILE_PATH"kinetic_error_test.csv"

#define CAMERA_WIDTH    640
#define CAMERA_HEIGHT   480

#define CYCLES          1000

#define P_X_STEP        200
#define P_X_MIN         P_X_STEP
#define P_X_MAX         CAMERA_WIDTH - P_X_STEP

#define P_Y_STEP        1 // P_X_STEP
#define P_Y_MIN         CAMERA_HEIGHT / 2 //P_Y_STEP
#define P_Y_MAX         P_Y_MIN

#define BOUNDU(X,MAX)           ( ( X > MAX ) ? MAX : X )         // Bound in upper range
#define BOUND(X,MIN,MAX)        ( ( X < MIN ) ? MIN : BOUNDU( X, MAX ) ) // Bound in upper and lower range

typedef double floating_t;

#endif /* test_config_h */
