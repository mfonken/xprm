//
//  psm.c
//  hmmtest
//
//  Created by Matthew Fonken on 2/12/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#include "psm.h"

void InitializePSM( psm_t * model )
{
    GMMFunctions.Model.Initialize( &model->gmm );
    KumaraswamyFunctions.Initialize( &model->kumaraswamy, NUM_STATES );
}

void UpdatePSM( psm_t * model, double nu )
{
    double state_intervals[NUM_STATE_GROUPS] = { 0. };
    PSMFunctions.UpdateStateIntervals( model, nu, state_intervals, NUM_STATE_GROUPS );
    
    /* Update states/transition matrix */
    BayesianFunctions.Sys.Update( &model->hmm.A, state_intervals );
    
    /* Update observation matrix */
    model->hmm.B.num_observation_symbols = PSMFunctions.GetNumberOfObservationGroups( model );
    double observation_scores[MAX_OBSERVATIONS] = { 0. };
    uint8_t observation_group = PSMFunctions.ScoreObservations( model, observation_scores, model->gmm.num_clusters );
    uint8_t estimated_state = PSMFunctions.GetStateFromObservationGroup( observation_group, state_intervals, NUM_STATE_GROUPS );
    
    for( uint8_t i = 0; i < MAX_OBSERVATIONS; i++ )
    {
        model->hmm.B.map[i][estimated_state] += observation_scores[i];
        model->hmm.B.map[i][estimated_state] /= 2.;
    }
    
    HMMFunctions.Update( &model->hmm );
}

void ReportObservationsPSM( psm_t * model, observation_t * observations, uint8_t num_observations )
{
    vec2 value;
    for( uint8_t i = 0; i < num_observations && i < MAX_OBSERVATIONS; i++ )
    {
        GMMFunctions.Model.Update( &model->gmm, &observations[i], &value );
        // Analyse value
    }
}

uint8_t GetNumberOfObservationGroupsPSM( psm_t * model )
{
    uint8_t num_observation_groups = 0;
    
    // Calculate number of groups
    
    return num_observation_groups;
}

void InfluenceStateBoundariesPSM( psm_t * model, double * boundaries, uint8_t num_boundaries )
{
    for( uint8_t i = 0; i < model->gmm.num_clusters; i++ )
    {
        
    }
}

void UpdateStateIntervalsPSM( psm_t * model, double nu, double * intervals, uint8_t num_intervals )
{
    double boundaries[NUM_STATE_GROUPS];
    for( uint8_t i = 1; i <= NUM_STATE_GROUPS; i++ )
        boundaries[i] = (double)i / NUM_STATE_GROUPS;
    PSMFunctions.InfluenceStateBoundaries( model, boundaries, NUM_STATE_GROUPS );
    KumaraswamyFunctions.GetVector( &model->kumaraswamy, nu, intervals, boundaries, NUM_STATE_GROUPS );
}

uint8_t ScoreObservationsPSM( psm_t * model, double * observation_scores, uint8_t num_scores )
{
    uint8_t observation_group = 0;
    // Calculate scores
    
    return observation_group;
}

uint8_t GetStateFromObservationGroupPSM( uint8_t observation_group, double * intervals, uint8_t num_intervals )
{
    uint8_t state = 0;
    return state;
}
