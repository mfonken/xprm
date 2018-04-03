#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#define MAX_PEAKS 1 << 3
#define IMAGE_DIMENSION 1 << 10

typedef uint16_t dimension_t;
typedef uint8_t density_t;
typedef float float_t;

/** Kalman Uncertadensity_ties */
typedef struct
{
    float_t value;
    float_t bias;
    float_t sensor;
} kalman_uncertadensity_ty_t;

float_t now(void);

typedef struct
{
    density_t p[IMAGE_DIMENSION][IMAGE_DIMENSION];
} image_t;

/** Kalman class */
class KalmanFilter
{
public:  
    float_t      K[2],
                P[2][2],
                rate,
                bias,
                value,
                prev,
                velocity,
                lifespan,
                timestamp;
    kalman_uncertadensity_ty_t uncertadensity_ty;
    
    KalmanFilter();
    KalmanFilter( float_t );
    KalmanFilter( float_t, float_t );
    KalmanFilter( float_t, float_t, float_t, float_t, float_t );
    void update( float_t, float_t );
    void copyTo(KalmanFilter *);
    std::string toString();
    density_t  isExpired();
};


typedef struct
{
    float_t  primary,
    secondary,
    alternate;
} prediction_probabilities_t;

class Prediction
{
public:
    KalmanFilter    primary,
                    secondary;
    float_t         primary_new,
                    secondary_new;
    prediction_probabilities_t probabilities;
    
    Prediction();
};

typedef enum
{
    SIMILAR = 0,
    OPPOSITE
} selection_pair_t;

class PredictionPair
{
public:
    Prediction x;
    Prediction y;
    selection_pair_t        selection_pair;
};

class DensityMap
{
public:
    density_t       map[IMAGE_DIMENSION];
    dimension_t     length;
//    density_t       max;
//    float_t         variance;
    KalmanFilter    kalman;
};

class DensityMapPair
{
public:
    DensityMap   x;
    DensityMap   y;
    
    DensityMapPair( density_t, density_t );
};

class PeakList
{
public:
    dimension_t   length;
    density_t     map[MAX_PEAKS];
    density_t     den[MAX_PEAKS];
    density_t     max[MAX_PEAKS];
};

class PeakListPair
{
public:
    PeakList x;
    PeakList y;
    
    PeakListPair();
};

