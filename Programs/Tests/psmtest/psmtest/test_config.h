//
//  test_config.h
//  psmtest
//
//  Created by Matthew Fonken on 8/29/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef test_config_h
#define test_config_h

#include <stdint.h>

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

//#define RHO_DEBUG               DEBUG_2
//#define RHO_DEBUG_2             DEBUG_2
//#define KALMAN_DEBUG
//#define PACKET_DEBUG

#define PSM_DEBUG               DEBUG_2
#define PSM_DEBUG_2             DEBUG_2

#define HMM_DEBUG               ALWAYS
#define HMM_DEBUG_2             DEBUG_1
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

#ifdef RHO_DEBUG
#define LOG_RHO(L,...)          LOG(L,"<Rho> " __VA_ARGS__)
#define LOG_RHO_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_RHO(...)
#define LOG_RHO_BARE(L,...)
#endif

#ifdef KALMAN_DEBUG
#define LOG_KALMAN(L,...)       LOG(L,"<Kalman> " __VA_ARGS__)
#else
#define LOG_KALMAN(...)
#endif

#ifdef PACKET_DEBUG
#define LOG_PACKET(L,...)       LOG(L,"<Packet> " __VA_ARGS__)
#else
#define LOG_PACKET(...)
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

#ifndef MAX
#define MAX(A,B)                ( ( A > B ) ? A : B )
#endif

#define AVG2(A,B)               ( ( A + B ) / 2. )
#define SWAP(A,B)               { typeof(A) temp = A; A = B; B = temp; }

#define BOUNDU(X,MAX)           ( ( X > MAX ) ? MAX : X )         // Bound in upper range
#define BOUND(X,MIN,MAX)        ( ( X < MIN ) ? MIN : BOUNDU( X, MAX ) ) // Bound in upper and lower range

#define FBOUND(X,MIN,MAX)       ( ( X < MIN ) ? MIN : ( ( X > MAX ) ? MAX : X ) )

#define SQUARE(X)               ( X * X )
#define DISTANCE_SQ(X,Y)        ( SQUARE(X) + SQUARE(Y) )
#define INRANGE(X,Y,T)          ( abs( X - Y ) < T )

#ifndef ZDIV
#define ZDIV_LNUM               ( 1 << 10 )
#define ZDIV(X,Y)               ( ( Y == 0 ) ? ( X == 0 ? 0 : ZDIV_LNUM ) : X / Y )
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

#define DEFAULT_PROPORTIONAL_FACTOR 0.5
#define DEFAULT_INTEGRAL_FACTOR     0.002
#define DEFAULT_DERIVATIVE_FACTOR   0.001
#define DEFAULT_PID_GAIN (rho_pid_gain_t){ DEFAULT_PROPORTIONAL_FACTOR, DEFAULT_INTEGRAL_FACTOR, DEFAULT_DERIVATIVE_FACTOR, 0., 0. }

#endif /* test_config_h */
