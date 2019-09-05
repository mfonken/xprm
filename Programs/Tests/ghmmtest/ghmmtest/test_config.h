//
//  test_config.h
//  ghmmtest
//
//  Created by Matthew Fonken on 9/4/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef test_config_h
#define test_config_h

typedef uint16_t index_t;
typedef double floating_t;

#ifndef LOG_LEVEL
#define LOG_LEVEL
enum LogLevel
{
    TEST = 0,
    DEBUG_0,
    DEBUG_1,
    DEBUG_2,
    ALWAYS,
    OFF
};
#endif

#define PSM_DEBUG               DEBUG_2
#define PSM_DEBUG_2             DEBUG_2

#define HMM_DEBUG               ALWAYS
//#define GMM_DEBUG               DEBUG_2
#define FSM_DEBUG               DEBUG_2
#define FSM_DEBUG_2             DEBUG_2

#define TEST_LOG_LEVEL          DEBUG_2

#ifndef LOG
#define LOG(L,...)              if(L >= TEST_LOG_LEVEL) \
{  for(uint8_t i=L;i<ALWAYS;i++) \
printf("\t"); printf(__VA_ARGS__); }
#define LOG_BARE(L,...)         if(L >= TEST_LOG_LEVEL) \
{ printf(__VA_ARGS__); }
#endif

#ifdef PSM_DEBUG
#define LOG_PSM(L,...)          LOG(L,"<PSM> " __VA_ARGS__)
#define LOG_PSM_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_PSM(...)
#define LOG_PSM_BARE(...)
#endif

#ifdef HMM_DEBUG
#define LOG_HMM(L,...)          LOG(L,"<HMM> " __VA_ARGS__)
#define LOG_HMM_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_HMM(...)
#define LOG_HMM_BARE(...)
#endif

#ifdef GMM_DEBUG
#define LOG_GMM(L,...)          LOG(L,"<GMM> " __VA_ARGS__)
#define LOG_GMM_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_GMM(...)
#define LOG_GMM_BARE(L,...)
#endif

#ifdef FSM_DEBUG
#define LOG_FSM(L,...)          LOG(L,"<FSM> " __VA_ARGS__)
#define LOG_FSM_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_FSM(...)
#define LOG_FSM_BARE(L,...)
#endif

#ifndef BOUND
#define BOUNDU(X,MAX)           ( ( X > MAX ) ? MAX : X )         // Bound in upper range
#define BOUND(X,MIN,MAX)        ( ( X < MIN ) ? MIN : BOUNDU( X, MAX ) ) // Bound in upper and lower range
#endif

#endif /* test_config_h */
