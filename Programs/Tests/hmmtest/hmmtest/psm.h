//
//  psm.h
//  hmmtest
//
//  Created by Matthew Fonken on 2/12/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//
// Description: Predictive State Model

#ifndef psm_h
#define psm_h

#include <stdio.h>

#include "hmm.h"

void InitializePSM(                      psm_t * );
void UpdatePSM(                          psm_t *, double  );
void ReportObservationsPSM(              psm_t *, observation_t *, uint8_t );
uint8_t GetNumberOfObservationGroupsPSM( psm_t * );
void InfluenceStateBoundariesPSM(        psm_t *, double *, uint8_t );
void UpdateStateIntervalsPSM(            psm_t *, double , double *, uint8_t );
uint8_t ScoreObservationsPSM(            psm_t *, double *, uint8_t );
uint8_t GetStateFromObservationGroupPSM( uint8_t, double *, uint8_t );

typedef struct
{
    void (*Initialize)(                      psm_t * );
    void (*Update)(                          psm_t *, double  );
    void (*ReportObservations)(              psm_t *, observation_t *, uint8_t );
    uint8_t (*GetNumberOfObservationGroups)( psm_t * );
    void (*InfluenceStateBoundaries)(        psm_t *, double *, uint8_t );
    void (*UpdateStateIntervals)(            psm_t *, double , double *, uint8_t );
    uint8_t (*ScoreObservations)(            psm_t *, double *, uint8_t );
    uint8_t (*GetStateFromObservationGroup)( uint8_t, double *, uint8_t );
} psm_functions_t;

static const psm_functions_t PSMFunctions =
{
    .Initialize = InitializePSM,
    .Update = UpdatePSM,
    .ReportObservations = ReportObservationsPSM,
    .GetNumberOfObservationGroups = GetNumberOfObservationGroupsPSM,
    .InfluenceStateBoundaries = InfluenceStateBoundariesPSM,
    .UpdateStateIntervals = UpdateStateIntervalsPSM,
    .ScoreObservations = ScoreObservationsPSM,
    .GetStateFromObservationGroup = GetStateFromObservationGroupPSM
};

#endif /* psm_h */
