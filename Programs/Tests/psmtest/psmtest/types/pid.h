//
//  pid.h
//  rho_client
//
//  Created by Matthew Fonken on 10/21/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#ifndef pid_h
#define pid_h

#include <string.h>

#ifdef __RHO__
#include "rho_global.h"
#else
#include "test_config.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif
    
    typedef struct
    {
        floating_t Kp, Ki, Kd, Ku, Pu;
    } pid_gain_t;
    
    typedef struct
    {
      pid_gain_t
        Gain;
      floating_t
        Pv,
        Iv,
        Dv,
        Value,
        Bias,
        Dt,
        Timestamp,
        PrevError,
        Error,
        DeltaError,
        TotalError,
        MinValue,
        MaxValue;
    } pid_filter_t;
    
    typedef struct
    {
        void (*Initialize)( pid_filter_t *, pid_gain_t );
        void (*Update)( pid_filter_t *, floating_t, floating_t );
        void (*Print)( pid_filter_t * );
    } pid_functions;
    
    void RhoPIDInitialize( pid_filter_t *, pid_gain_t );
    void RhoPIDUpdate( pid_filter_t *, floating_t, floating_t );
    void RhoPIDPrint( pid_filter_t * );
    
    static const pid_functions RhoPID =
    {
        .Initialize = RhoPIDInitialize,
        .Update = RhoPIDUpdate,
        .Print = RhoPIDPrint
    };
    
#ifdef __cplusplus
}
#endif

#endif /* pid_h */
