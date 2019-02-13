//
//  kumaraswamy.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/11/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef kumaraswamy_h
#define kumaraswamy_h

#define KUMARASWAMY_CDF(X,A,B) ( 1 - pow( 1 - pow( X, A ), B) )

typedef struct
{
    double alpha, beta;
} kumaraswamy_t;

void InitializeKumaraswamy( kumaraswamy_t * k, double alpha, double beta )
{
    k->alpha = alpha;
    k->beta = beta;
}

static inline double PerformKumaraswamyCDF( kumaraswamy_t * k, double x )
{
    return KUMARASWAMY_CDF( x, k->alpha, k->beta );
}

static void GetKumaraswamyVector( kumaraswamy_t * k, double alpha, double * interval, double * boundary, uint8_t num_intervals ) // boundaries are between 0 to 1
{
    k->alpha = alpha;
    double curr_CDF, prev_CDF = 0.;
    for( uint8_t i = 0; i < num_intervals; i++ )
    {
        curr_CDF = PerformKumaraswamyCDF( k, boundary[i] );
        interval[i] = curr_CDF - prev_CDF;
        prev_CDF = curr_CDF;
    }
}

#endif /* kumaraswamy_h */
