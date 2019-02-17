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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#include "hmm.h"

void InitializePSM(                      psm_t * );
void ReportObservationsPSM(              psm_t *, observation_t *, uint8_t );
void UpdatePSM(                          psm_t *, double  );
void UpdateStateIntervalsPSM(            psm_t *, double , double *, uint8_t );
void InfluenceStateBandsPSM(             psm_t *, band_list_t * );
void FindLowerBoundariesOfStateBandPSM(  psm_t *, cluster_boundary_list_t *, band_list_t * );
void FindTrueCentersOfStateBandsPSM(     psm_t *, cluster_boundary_list_t *, band_list_t * );
uint8_t FindBestClusterPSM(              psm_t * );
uint8_t GetCurrentBandPSM(               psm_t *, band_list_t * );

void InitializePSM( psm_t * model );
void ReportObservationsPSM( psm_t * model, observation_t * observations, uint8_t num_observations );
void UpdatePSM( psm_t * model, double nu );
void UpdateStateIntervalsPSM( psm_t * model, double nu, double * intervals, uint8_t num_intervals );
void InfluenceStateBandsPSM( psm_t * model, band_list_t * band_list );
void FindLowerBoundariesOfStateBandPSM( psm_t * model, cluster_boundary_list_t * cluster_boundaries, band_list_t * band_list );
void FindTrueCentersOfStateBandsPSM( psm_t * model, cluster_boundary_list_t * cluster_boundaries, band_list_t * band_list );
uint8_t FindBestClusterPSM( psm_t * model );
uint8_t GetCurrentBandPSM( psm_t * model, band_list_t * band_list );

typedef struct
{
    void (*Initialize)(                      psm_t * );
    void (*ReportObservations)(              psm_t *, observation_t *, uint8_t );
    void (*Update)(                          psm_t *, double  );
    void (*UpdateStateIntervals)(            psm_t *, double , double *, uint8_t );
    void (*InfluenceStateBands)(             psm_t *, band_list_t * );
    void (*FindStateBandLowerBoundaries)(    psm_t *, cluster_boundary_list_t *, band_list_t * );
    void (*FindStateBandCenters)(            psm_t *, cluster_boundary_list_t *, band_list_t * );
    uint8_t (*FindBestCluster)(              psm_t * );
    uint8_t (*GetCurrentBand)(            psm_t *, band_list_t * );
} psm_functions_t;

static const psm_functions_t PSMFunctions =
{
    .Initialize = InitializePSM,
    .ReportObservations = ReportObservationsPSM,
    .Update = UpdatePSM,
    .UpdateStateIntervals = UpdateStateIntervalsPSM,
    .InfluenceStateBands = InfluenceStateBandsPSM,
    .FindStateBandLowerBoundaries = FindLowerBoundariesOfStateBandPSM,
    .FindStateBandCenters = FindTrueCentersOfStateBandsPSM,
    .FindBestCluster = FindBestClusterPSM,
    .GetCurrentBand = GetCurrentBandPSM,
};
    
#ifdef __cplusplus
}
#endif

#endif /* psm_h */
