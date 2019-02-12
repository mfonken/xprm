//
//  state_machine_utility.hpp
//  tau+
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2018 Marbl. All rights reserved.
//

#ifndef state_machine_utility_h
#define state_machine_utility_h

#include <stdint.h>
#include <stdbool.h>

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
    
#ifndef ZDIV
#define ZDIV_LNUM 1 << 10
#define ZDIV(X,Y) ((Y==0)?(X==0?0:ZDIV_LNUM):X/Y)
#endif
    
#ifndef TEST_LOG_LEVEL
#define TEST_LOG_LEVEL ALWAYS
#endif
    
#ifndef LOG
#define LOG(L,...) if(L >= TEST_LOG_LEVEL) \
{  for(uint8_t i=L;i<ALWAYS;i++) printf("\t"); printf(__VA_ARGS__); }
#endif
    
#ifdef STATEM_DEBUG
#define LOG_STATEM(L,...) LOG(L,"<State> " __VA_ARGS__)
#else
#define LOG_STATEM(...)
#endif
    
    /***************************************************************************************/
    /*                          DEFINITIONS & MACROS                                       */
    /***************************************************************************************/
#define PROBABILITY_TUNING_FACTOR       0.5
#define PROBABILITY_TUNING_FACTOR_SQ    ( PROBABILITY_TUNING_FACTOR * PROBABILITY_TUNING_FACTOR )
#define PROBABILITY_THRESHOLD           0.1
#define PROBABILITY_ALTERNATE_THRESH    0.9
#define ABSENCE_FACTOR                  PROBABILITY_TUNING_FACTOR
#define PROBABILITY_TUNING_THRESHOLD    ( PROBABILITY_THRESHOLD * PROBABILITY_THRESHOLD )
#define PROBABILITY_STABILITY_THRESHOLD 0.5
#define STATE_DISTANCE                  2.0
#define SHADOW_TOLERANCE                0.2
#define DOUBT_STABILITY                 0.5
#define STATE_DECAY                     0.95
#define STATE_PUNISH                    0.025
#define STABILITY_FACTOR                3.0
    
    /***************************************************************************************/
    
    /** Goals **
     *  - Column is current state
     *  - Row is next state
     *  - Diagonal is probability of remaining in state, should tend to be most probable
     *  - System should try to stay in S2
     *  - All probabilities are <= 1.0 (100%) and...
     *  - ...all row probabilities add to <= 1.0 (100%)
     */
    
    /* Tau states */
#define state_dimension_t uint8_t
#define loop_variables_state_dimension_t loop_variables_uint8_t //##state_dimension_t
    
    typedef enum
    {
        UNKNOWN_STATE = 0,
        STABLE_NONE,
        UNSTABLE_NONE,
        STABLE_SINGLE,
        UNSTABLE_SINGLE,
        STABLE_DOUBLE,
        UNSTABLE_DOUBLE,
        STABLE_MANY,
        UNSTABLE_MANY,
        NUM_STATES
    } state_t;
    
    #define NUM_STATE_GROUPS ((uint8_t)NUM_STATES/2)
    
    static inline state_dimension_t stateToSelection(state_t s) {return ((state_dimension_t)((s+1)/2) - 1);};
    static inline const char *stateString(state_dimension_t s)
    {
        static const char *strings[] = { "UN", "S0", "U0", "S1", "U1", "S2", "U2", "SM", "UM" };
        return strings[(state_dimension_t)s];
    }
    
    /* Stability tracking for selec tions */
    typedef struct
    {
        double primary;
        double secondary;
        double alternate;
        double overall;
    } stability_t;
    
    /* Bayesian state tree with bayesian base */
    typedef struct
    {
        double map[NUM_STATES][NUM_STATES];
        state_dimension_t length;
    } bayesian_map_t;
    
    /* System self-diagnostic state control type */
    typedef struct
    {
        state_t         state;
        state_t         prev;
        state_t         next;
        state_dimension_t  selection_index;
        stability_t     stability;
        bayesian_map_t  probabilities;
    } bayesian_system_t;
    
    struct bayesian_map_functions
    {
        void (*InitializeMap)(       bayesian_map_t * );
        void (*NormalizeMap)(        bayesian_map_t * );
        void (*NormalizeState)(      bayesian_map_t *, state_dimension_t );
        void (*ResetState)(          bayesian_map_t *, state_dimension_t );
        void (*Print)(               bayesian_map_t *, state_t s );
    };
    
    struct bayesian_system_functions
    {
        void (*Initialize)(          bayesian_system_t *                );
        void (*DecayInactive)(       bayesian_system_t *                );
        void (*UpdateProbabilities)( bayesian_system_t *, double[4] );
        void (*UpdateState)(         bayesian_system_t *                );
        void (*Update)(              bayesian_system_t *, double[4] );
    };
    
    struct bayesian_functions
    {
        struct bayesian_map_functions    Map;
        struct bayesian_system_functions Sys;
    };
    
    void InitializeBayesianMap(        bayesian_map_t *                            );
    void NormalizeBayesianMap(         bayesian_map_t *                            );
    void NormalizeBayesianState(       bayesian_map_t *,       state_dimension_t   );
    void ResetBayesianState(           bayesian_map_t *,       state_dimension_t   );
    void PrintBayesianMap(             bayesian_map_t *,       state_t             );
    void InitializeBayesianSystem(     bayesian_system_t *                         );
    void DecayInactiveBayesianSystem(  bayesian_system_t *                         );
    void UpdateBayesianSystem(         bayesian_system_t *,    double[4]       );
    void UpdateBayesianProbabilities(  bayesian_system_t *,    double[4]       );
    void UpdateBayesianState(          bayesian_system_t *                         );
    
    static const struct bayesian_functions BayesianFunctions =
    {
        { /* Map functions */
            .InitializeMap          = InitializeBayesianMap,
            .NormalizeMap           = NormalizeBayesianMap,
            .NormalizeState         = NormalizeBayesianState,
            .ResetState             = ResetBayesianState,
            .Print                  = PrintBayesianMap
        },
        { /* System functions */
            .Initialize             = InitializeBayesianSystem,
            .DecayInactive          = DecayInactiveBayesianSystem,
            .Update                 = UpdateBayesianSystem,
            .UpdateProbabilities    = UpdateBayesianProbabilities,
            .UpdateState            = UpdateBayesianState
        }
    };
    
    static inline void copymax(double * a, double * b) { if(*a>*b)*b=*a;else*a=*b; }
    static inline bool isStable( state_t s ) { return ((state_dimension_t)s % 2); }
    static inline state_dimension_t stateNumber( state_t s ) { return (state_dimension_t)( ( s - 1 ) / 2 ); }
    
#define define_loop_variable_template_struct(T, N)struct { T l, i, j; double u, v;}N;
    
#ifdef __cplusplus
}
#endif

#endif /* state_machine_utility_hpp */
