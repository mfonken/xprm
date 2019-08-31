#ifndef kalman_h
#define kalman_h

#include <stdlib.h>
#include <stdbool.h>

#ifdef __RHO__
#include "rho_global.h"
#endif

#define KALMAN_PUNISH_FACTOR  0.7
#define MIN_KALMAN_GAIN       0.001
#define KALMAN_MATURATION     3 // Seconds

#ifdef __cplusplus
extern "C" {
#endif
    
    /** Kalman Uncertainties */
    typedef struct
    {
        floating_t value;
        floating_t bias;
        floating_t sensor;
    } kalman_uncertainty_c;
    
    /** Kalman structure */
    typedef struct
    {
        floating_t
        K[2],
        P[2][2],
        rate,
        bias,
        value,
        prev,
        velocity,
        variance,
        lifespan,
        timestamp,
        origin,
        score;
        kalman_uncertainty_c
        uncertainty;
        bool
        sorted,
        valid,
        flag;
        index_t
        min_value,
        max_value;
    } kalman_filter_t;
    
    void  InitializeRhoKalman( kalman_filter_t *, floating_t, floating_t, index_t, index_t, kalman_uncertainty_c );
    void       ResetRhoKalman( kalman_filter_t *, floating_t );
    void     PredictRhoKalman( kalman_filter_t *, floating_t );
    void      UpdateRhoKalman( kalman_filter_t *, floating_t );
    floating_t  StepRhoKalman( kalman_filter_t *, floating_t, floating_t );
    bool   IsRhoKalmanExpired( kalman_filter_t * );
    floating_t ScoreRhoKalman( kalman_filter_t * );
    void      PunishRhoKalman( kalman_filter_t * );
    
    struct kalman {
        void (*  Initialize)( kalman_filter_t *, floating_t, floating_t, index_t, index_t, kalman_uncertainty_c );
        void (*      Reset)( kalman_filter_t *, floating_t );
        void (*    Predict)( kalman_filter_t *, floating_t );
        void (*     Update)( kalman_filter_t *, floating_t );
        floating_t (* Step)( kalman_filter_t *, floating_t, floating_t );
        bool (*  IsExpired)( kalman_filter_t * );
        floating_t (*Score)( kalman_filter_t * );
        void (*     Punish)( kalman_filter_t * );
    };
    
    static const struct kalman Kalman =
    {
        .Initialize = InitializeRhoKalman,
        .Reset = ResetRhoKalman,
        .Predict = PredictRhoKalman,
        .Update = UpdateRhoKalman,
        .Step = StepRhoKalman,
        .IsExpired = IsRhoKalmanExpired,
        .Score = ScoreRhoKalman,
        .Punish = PunishRhoKalman
    };
    
#ifdef __cplusplus
}
#endif

#endif /* kalman_h */

