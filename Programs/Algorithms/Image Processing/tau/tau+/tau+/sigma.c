//
//  sigma.c
//  tau+
//
//  Created by Matthew Fonken on 8/22/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "sigma.h"

image_dimension_base_t radius;

void performSigma( prediction_pair_t * prediction, prediction_pair_t * last, prediction_pair_t * result  )
{
    radius = getRadius(prediction);
    getExtrapolation(&prediction->x, &last->x, &result->x);
    getExtrapolation(&prediction->y, &last->y, &result->y);
}

image_dimension_base_t getRadius( prediction_pair_t * pair)
{
    image_dimension_base_t a, b;
    a = pair->x.secondary - pair->x.primary;
    b = pair->y.secondary - pair->y.primary;
    return sqrt( ( a * a ) + ( b * b ) );
}

FLOAT getRotationCoupling( prediction_t * prediction, prediction_t * last )
{
    image_dimension_base_t a, b;
    a = last->primary   - prediction->primary;
    b = last->secondary - prediction->secondary;
    return ( ( b + a ) / ( b - a ) );
}

void getExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result )
{
    FLOAT coupling;
    prediction_t linear_extrapolation, angular_extrapolation;
    getLinearExtrapolation( prediction, last, &linear_extrapolation );
    getAngularExtrapolation( prediction, last, &angular_extrapolation );
    coupling = getRotationCoupling( prediction, last );
    result->primary   = ( coupling * ( linear_extrapolation.primary   - angular_extrapolation.primary   ) ) - angular_extrapolation.primary;
    result->secondary = ( coupling * ( linear_extrapolation.secondary - angular_extrapolation.secondary ) ) - angular_extrapolation.secondary;
}

void getLinearExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result )
{
    result->primary   = ( 2 * prediction->primary   ) - last->primary;
    result->secondary = ( 2 * prediction->secondary ) - last->secondary;
}

void getAngularExtrapolation( prediction_t * prediction, prediction_t * last, prediction_t * result )
{
    FLOAT primary_predicted_angle, primary_last_angle, secondary_predicted_angle, secondary_last_angle;
    primary_predicted_angle   = acos( prediction->primary   / radius );
    primary_last_angle        = acos( last->primary         / radius );
    result->primary           = radius * ( ( 2 * primary_predicted_angle ) - primary_last_angle );
    secondary_predicted_angle = acos( prediction->secondary / radius );
    secondary_last_angle      = acos( last->secondary       / radius );
    result->secondary         = radius * ( ( 2 * secondary_predicted_angle ) - secondary_last_angle );
}
