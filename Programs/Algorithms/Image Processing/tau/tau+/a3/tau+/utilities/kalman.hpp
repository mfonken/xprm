#ifndef kalman_hpp
#define kalman_hpp

//#define TIMELESS

/* Standard headers */
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>

#define DEFAULT_LS      5
#define DEFAULT_VU      0.001
#define DEFAULT_BU      0.5
#define DEFAULT_SU      0.7

/** Kalman Uncertainties */
typedef struct 
{
    double value;
    double bias;
    double sensor;
} kalman_uncertainty_t;

double now(void);

/** Kalman class */
class KalmanFilter
{
public:
    double lifespan;
    kalman_uncertainty_t uncertainty;

    double      K[2],
                P[2][2],
                rate,
                bias,
                value,
                last_update,
                prev,
                density,
                velocity;
    double      timestamp;
    
    KalmanFilter();
    void init( double );
    void init( double, double );
    void init( double, double, double, double, double );
    void update( double, double );
    std::string toString();
    int  isExpired();
};




#endif /* kalman_h */

