//
//  fsm.cpp
//  Finite State Machine
//
//  Created by Matthew Fonken on 2/8/18.
//  Copyright © 2019 Marbl. All rights reserved.
//

#include "fsm.h"

void InitializeFSMMap( transition_matrix_t * P )
{
    LOG_FSM(FSM_DEBUG, "Initializing State Machine.\n");
    for(uint8_t i = 0; i < NUM_STATES; i++ )
        FSMFunctions.Map.ResetState( P, i );
}

void ResetFSMState( transition_matrix_t * P, uint8_t i )
{
    for( uint8_t j = 0; j < NUM_STATES; j++ )
        (*P)[i][j] = 0.0;
    (*P)[i][i] = 1.0;
}

void NormalizeFSMMap( transition_matrix_t * P )
{
    for( uint8_t i = 0; i < NUM_STATES; i++ )
        FSMFunctions.Map.NormalizeState( P, i );
}

uint8_t NormalizeFSMState( transition_matrix_t * P, uint8_t i )
{
    uint8_t max_index = i, j;
    double total = 0, invtotal, curr, max = 0;
    for( j = 0; j < NUM_STATES; j++ )
    {
        curr = (*P)[i][j];
        if( curr >= 0. )
            total += curr;
    }
    if(total)
    {
        invtotal = ZDIV( 1., total );
        for( j = 0; j < NUM_STATES; j++ )
        {
            (*P)[i][j] *= invtotal;
            if( (*P)[i][j] > max )
            {
                max = (*P)[i][j];
                max_index = j;
            }
        }
    }
    else (*P)[i][i] = 1.0;
    
    return max_index;
}

void InitializeFSMSystem( fsm_system_t * sys, state_t initial_state )
{
    sys->state                = initial_state;
    sys->prev                 = UNKNOWN_STATE;
    sys->next                 = UNKNOWN_STATE;
    sys->selection_index      = 0;
#ifdef __FILTERS__
    Kalman.Initialize( &sys->stability.system, 0., FSM_LIFESPAN, 0., 1., FSM_STABLIITY_UNCERTAINTY );
    Kalman.Initialize( &sys->stability.state, 0., FSM_STATE_LIFESPAN, 0., 1., FSM_STATE_UNCERTAINTY );
#endif
    FSMFunctions.Map.Initialize( sys->P );
}

void DecayInactiveFSMSystem( fsm_system_t * sys )
{
    state_t c = sys->state;
    if( c == UNKNOWN_STATE ) return;
    for( uint8_t i = 0, j; i < NUM_STATES; i++ )
    {
        uint8_t state_distance = abs( i - c );
        for( j = 0; j < NUM_STATES; j++ ) // Punish based on relevance(distance) from current state
        {
            (*sys->P)[i][j] -= STATE_PUNISH * state_distance;
            if( (*sys->P)[i][j] < 0 ) (*sys->P)[i][j] = 0.;
        }
    }
}

void UpdateFSMSystem( fsm_system_t * sys, double p[4] )
{
    FSMFunctions.Sys.UpdateProbabilities( sys, p );
    
    /* Normalize state and find most probable next state */
    sys->next = (state_t)FSMFunctions.Map.NormalizeState( sys->P, sys->state );
    
    FSMFunctions.Sys.UpdateState( sys );
    PrintFSMMap( sys->P, sys->state );
#ifdef __FILTERS__
    sys->stability.system.timestamp = TIMESTAMP();
    sys->stability.state.timestamp = TIMESTAMP();
#endif
}

void UpdateFSMProbabilities( fsm_system_t * sys, double p[4] )
{
    state_t c = sys->state;

#ifdef __FILTERS__
#ifdef FSM_DEBUG
    LOG_FSM( FSM_DEBUG, "Probabilies are [0]%.2f [1]%.2f [2]%.2f [3]%.2f.\n", p[0], p[1], p[2], p[3]);
    LOG_FSM( FSM_DEBUG, "State %s has stability %.4f\n", stateString(c), sys->stability.state.value );
#endif
    floating_t curr = 0;
    for( uint8_t i = 0; i < NUM_STATES; i++ )
    {
        LOG_FSM(FSM_DEBUG, "Updating %s by %.2f.\n", stateString(i), p[i]);
        curr = WeightedAverage( (*sys->P)[c][i], p[i], ( sys->stability.state.value + 1 ) / 2 );
        if( curr <= MAX_SINGLE_CONFIDENCE )
            (*sys->P)[c][i] = curr;
    }
    floating_t state_change_rate = TIMESTAMP() - sys->stability.state.timestamp;
    Kalman.Step( &sys->stability.state, p[sys->state], state_change_rate );
#endif
}

void UpdateFSMState( fsm_system_t * sys )
{
    /* State change */
    if(sys->next != UNKNOWN_STATE )
    {
        LOG_FSM(FSM_DEBUG, "Updating state from %s to %s\n", stateString((int)sys->state), stateString((int)sys->next));
        if(sys->next != sys->state) {LOG_FSM(FSM_DEBUG, "~~~ State is %s ~~~\n", stateString(sys->next));}
        sys->prev   = sys->state;
        sys->state  = sys->next;
        sys->next   = UNKNOWN_STATE;
        
#ifdef __FILTERS__
        Kalman.Reset( &sys->stability.state, 0. );
        floating_t system_change_rate = TIMESTAMP() - sys->stability.system.timestamp;
        Kalman.Step( &sys->stability.system, (*sys->P)[sys->state][sys->state], system_change_rate );
#endif
        
#ifdef FSM_DECAY_INACTIVE
        FSMFunctions.Sys.DecayInactive( sys );
#endif
    }
}

void PrintFSMMap( transition_matrix_t * P, state_t s )
{
#ifdef FSM_DEBUG
    LOG_FSM(FSM_DEBUG_2, "\tn+1   n-> " );// "\t\t\t  ");
    for( uint8_t i = 0; i < NUM_STATES; i++ ) LOG_FSM_BARE(FSM_DEBUG_2, "%s-[%d]\t  ", stateString((uint8_t)i), i);
    LOG_FSM_BARE(FSM_DEBUG_2, "\n");
    for( uint8_t i = 0, j; i < NUM_STATES; i++ )
    {
        LOG_FSM(FSM_DEBUG_2, " %s-[%d]", stateString((uint8_t)i), i);
        for( j = 0; j < NUM_STATES; j++ )
        {
            LOG_FSM_BARE(FSM_DEBUG_2, "\t%c[%.5f]%c", ((j==(uint8_t)s)?'|':' '), (*P)[j][i], ((j==(uint8_t)s)?'|':' '));
        }
        LOG_FSM_BARE(FSM_DEBUG_2, "\n");
    }
    LOG_FSM_BARE(FSM_DEBUG_2, "\n");
#endif
}
