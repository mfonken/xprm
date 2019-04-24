//
//  fsm_support.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/16/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef fsm_support_h
#define fsm_support_h

#include "kumaraswamy.h"

#ifdef __cplusplus
extern "C" {
#endif    
    
#ifndef LOG_LEVEL
#define LOG_LEVEL
    enum LogLevel
    {
        TEST = 0,
        DEBUG_0,
        DEBUG_1,
        DEBUG_2,
        ALWAYS
    };
#endif
    
static inline uint8_t stateToSelection(uint8_t s) {return ((uint8_t)((s+1)/2) - 1);};
static inline const char *stateString(int8_t s)
{
    s++;
    static const char *strings[] = { "UN", "UP", "TP", "OP", "CH" };
    return strings[(uint8_t)s];
}
    
#ifdef __cplusplus
}
#endif

#endif /* fsm_support_h */
