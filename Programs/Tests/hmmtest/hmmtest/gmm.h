//
//  gmm.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef gmm_h
#define gmm_h

#include "control_types.h"

/* Cluster functions */
void InitializeGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, observation_t *, vec2 *, double );
void UpdateGaussianMixtureCluster(                     gaussian_mixture_cluster_t *, observation_t *, vec2 * );
void GetScoreOfGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, vec2 *);
void UpdateNormalOfGaussianMixtureCluster(             gaussian_mixture_cluster_t * );
void UpdateInputProbabilityOfGaussianMixtureCluster(   gaussian_mixture_cluster_t *, double );
void ContributeToOutputOfGaussianMixtureCluster(       gaussian_mixture_cluster_t *, vec2 *, vec2 * );

/* Model functions */
void InitializeGaussianMixtureModel(                   gaussian_mixture_model_t * );
double GetScoreSumOfClustersInGaussianMixtureModel(    gaussian_mixture_model_t *, vec2 * );
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
double GetMaxErrorOfGaussianMixtureModel(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
void AddClusterToGaussianMixtureModel(                 gaussian_mixture_model_t *, observation_t *, vec2 * );
void UpdateGaussianMixtureModel(                       gaussian_mixture_model_t *, observation_t *, vec2 * );
void AddValueToGaussianMixtureModel(                   gaussian_mixture_model_t *, observation_t *, vec2 * );

typedef struct
{
    void (*Initialize)(                 gaussian_mixture_cluster_t *, observation_t *, vec2 *, double );
    void (*Update)(                     gaussian_mixture_cluster_t *, observation_t *, vec2 * );
    void (*GetScore)(                   gaussian_mixture_cluster_t *, vec2 *);
    void (*UpdateNormal)(               gaussian_mixture_cluster_t * );
    void (*UpdateInputProbability)(     gaussian_mixture_cluster_t *, double );
    void (*ContributeToOutput)(         gaussian_mixture_cluster_t *, vec2 *, vec2 * );
} gaussian_mixture_cluster_functions;

typedef struct
{
    void   (*Initialize)(               gaussian_mixture_model_t * );
    double (*GetScoreSumOfClusters)(    gaussian_mixture_model_t *, vec2 * );
    double (*GetOutputAndBestDistance)( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
    double (*GetMaxError)(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
    void   (*AddCluster)(               gaussian_mixture_model_t *, observation_t *, vec2 * );
    void   (*Update)(                   gaussian_mixture_model_t *, observation_t *, vec2 * );
    void   (*AddValue)(                 gaussian_mixture_model_t *, observation_t *, vec2 * );
} gaussian_mixture_model_functions;

typedef struct
{
    gaussian_mixture_cluster_functions Cluster;
    gaussian_mixture_model_functions Model;
} gaussian_mixture_functions;

static gaussian_mixture_functions GMMFunctions =
{
    .Cluster.Initialize             = InitializeGaussianMixtureCluster,
    .Cluster.Update                 = UpdateGaussianMixtureCluster,
    .Cluster.GetScore               = GetScoreOfGaussianMixtureCluster,
    .Cluster.UpdateNormal           = UpdateNormalOfGaussianMixtureCluster,
    .Cluster.UpdateInputProbability = UpdateInputProbabilityOfGaussianMixtureCluster,
    .Cluster.ContributeToOutput     = ContributeToOutputOfGaussianMixtureCluster,
    
    .Model.Initialize               = InitializeGaussianMixtureModel,
    .Model.GetScoreSumOfClusters    = GetScoreSumOfClustersInGaussianMixtureModel,
    .Model.GetOutputAndBestDistance = GetOutputAndBestDistanceOfGaussianMixtureModel,
    .Model.GetMaxError              = GetMaxErrorOfGaussianMixtureModel,
    .Model.AddCluster               = AddClusterToGaussianMixtureModel,
    .Model.Update                   = UpdateGaussianMixtureModel,
    .Model.AddValue                 = AddValueToGaussianMixtureModel,
};

#endif /* gmm_h */
