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
typedef struct _kalman_uncertainty_t
{
    double value;
    double bias;
    double sensor;
} kalman_uncertainty_t;

typedef enum
{
    VELOCITY = 0,
    ACCELERATION
} update_type_t;

double thisTime(void);

/** Kalman class */
class Kalman
{
public:
    double lifespan;
    kalman_uncertainty_t uncertainty;

    double      K[2],
                P[2][2],
                rate,
                bias,
                value,
                prev,
                density,
                velocity;
    double      timestamp;
    
    void init( double );
    void init( double, double );
    void init( double, double, double, double, double );
    void update( double, double );
    void update( double, double, update_type_t );
    std::string toString();
    int  isExpired();
};




#endif /* kalman_h */

