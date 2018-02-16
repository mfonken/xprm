#include "kalman.hpp"

/** SOURCE: http://preview.tinyurl.com/9djhrem */

//#define TIMELESS

double now()
{
    struct timeval stamp;
    gettimeofday(&stamp, NULL);
    return stamp.tv_sec + stamp.tv_usec/1000000.0;
}

KalmanFilter::KalmanFilter()
{
    init(0.);
}

void KalmanFilter::init( double v )
{
    init(v, DEFAULT_LS, DEFAULT_VU, DEFAULT_BU, DEFAULT_SU );
}

void KalmanFilter::init( double v, double ls )
{
    init(v, ls, DEFAULT_VU, DEFAULT_BU, DEFAULT_SU );
}

void KalmanFilter::init( double v, double ls, double v_u, double b_u, double s_u )
{
    K[0]        = 0;
    K[1]        = 0;
    P[0][0]     = 0;
    P[0][1]     = 0;
    P[1][0]     = 0;
    P[1][1]     = 0;
    rate        = 0;
    bias        = 0;
    value       = v;
    prev        = 0;
    velocity    = 0;
    density     = 0;
    
    last_update = 0;

    lifespan    = ls;
    uncertainty.value   = v_u;
    uncertainty.bias    = b_u;
    uncertainty.sensor  = s_u;
}

void KalmanFilter::update( double value_new, double rate_new )
{
#ifdef TIMELESS
//    double delta_time = 0.25;
#else
    double delta_time = now() - timestamp;

    /* Quick expiration check */
    if(delta_time > lifespan)
    {
        timestamp = now();
        return;
    }
#endif
    last_update = value_new;
    prev       = value;
    rate       = rate_new - bias;
    value     += delta_time * rate;

    double dt_P_1_1 = delta_time * P[1][1];
    P[0][0] +=   delta_time * ( dt_P_1_1 -
                                   P[0][1] -
                                   P[1][0] +
                                   uncertainty.value );
    P[0][1] -=   dt_P_1_1;
    P[1][0] -=   dt_P_1_1;
    P[1][1] +=   uncertainty.bias * delta_time;

    double S      = P[0][0] + uncertainty.sensor;
    K[0]       = P[0][0] / S;
    K[1]       = P[1][0] / S;
    double delta_value = value_new - value;
    value     += K[0] * delta_value;
    bias      += K[1] * delta_value;

    P[0][0] -= K[0] * P[0][0];
    P[0][1] -= K[0] * P[0][1];
    P[1][0] -= K[1] * P[0][0];
    P[1][1] -= K[1] * P[0][1];

    velocity = rate;
    timestamp = now();
};

std::string KalmanFilter::toString()
{
    char buff[100];
    snprintf(buff, sizeof(buff), "P[%.3f][%.3f][%.3f][%.3f] K[%.3f][%.3f] R[%.3f] B[%.3f] V[%.3f]", P[0][0], P[0][1], P[1][0], P[1][1], K[0], K[1], rate, bias, value);
    
    std::string ret = buff;
    return ret;
}

int KalmanFilter::isExpired()
{
    return ((now() - timestamp) > lifespan);
}
