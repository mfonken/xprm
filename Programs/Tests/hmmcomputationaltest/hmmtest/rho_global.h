/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  File: rho_global.h
 *  Group: Rho Core
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef rho_global_h
#define rho_global_h

#ifdef __OV9712__
#include "OV9712.h"
#else
#ifdef __PRINTERS__
#include "printers.h"
#endif
#endif

//#include "test_config.h"
//#include "genlog.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef double          floating_t;
typedef uint8_t         byte_t;
typedef uint16_t        index_t;
typedef uint16_t        density_t;
typedef int16_t         sdensity_t;
typedef uint16_t        density_map_unit_t;
typedef int16_t         variance_t;
typedef uint32_t        density_2d_t;
typedef floating_t      timestamp_t;
typedef uint8_t         capture_t;

#if defined __linux || defined __APPLE__
typedef void *          address_t;
#else
typedef uint32_t        address_t;
#endif

#define byte_t_max              ( (sizeof(byte_t)       << 3 ) - 1 )
#define index_t_max             ( (sizeof(index_t)      << 3 ) - 1 )
#define density_t_max           ( (sizeof(density_t)    << 3 ) - 1 )
#define density_2d_t_max        ( (sizeof(density_2d_t) << 3 ) - 1 )
#define density_map_unit_t_max  ( (sizeof(density_map_unit_t) << 3 ) - 1 )

//#define PSM_DEBUG               DEBUG_2
//#define PSM_DEBUG_2             DEBUG_1
//#define PSM_DEBUG_UPDATE        PSM_DEBUG
#define HMM_DEBUG               //RHO_DEBUG
#define HMM_REPORT              //RHO_DEBUG_2
//#define GMM_DEBUG               PSM_DEBUG
//#define GMM_DEBUG_2             PSM_DEBUG_2
//#define GMM_DEBUG_CLUSTERS      PSM_DEBUG_2
//#define FSM_DEBUG               RHO_DEBUG
//#define FSM_DEBUG_2             RHO_DEBUG
//#define FSM_DEBUG_UPDATE        RHO_DEBUG
//#define FSM_DEBUG_PRINT         RHO_DEBUG

//#define PACKET_DEBUG            RHO_DEBUG
//#define PACKET_DEBUG_2          RHO_DEBUG_2

#ifdef PSM_DEBUG
#define LOG_PSM(L,...)          LOG(L,"<PSM> " __VA_ARGS__)
#define LOG_PSM_BARE(L,...)     LOG_BARE(L,"" __VA_ARGS__)
#else
#define LOG_PSM(...)
#define LOG_PSM_BARE(...)
#endif

#ifdef HMM_DEBUG
#define LOG_HMM(L,...)          printf("<HMM> " __VA_ARGS__) // LOG(L,"<HMM> " __VA_ARGS__)
#define LOG_HMM_BARE(L,...)     printf("" __VA_ARGS__) //LOG_BARE(L,"" __VA_ARGS__)
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

#endif /* rho_global_h */
