//
//  sigma.h
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#ifdef __cplusplus
extern "C" {
#endif

#ifndef sigma_h
#define sigma_h

#include <stdio.h>
#include <math.h>
#include "global_types.h"

extern image_dimension_base_t radius;

void performSigma( prediction_pair_t * prediction, prediction_pair_t * last, prediction_pair_t * result  );
image_dimension_base_t getRadius( prediction_pair_t * pair);
FLOAT getRotationCoupling( prediction_t * prediction, prediction_t * last );
void getExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result );
void getLinearExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result );
void getAngularExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result );

#endif /* sigma_h */

#ifdef __cplusplus
}
#endif
