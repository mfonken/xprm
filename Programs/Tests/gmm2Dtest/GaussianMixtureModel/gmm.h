//
//  gmm.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/9/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef gmm_h
#define gmm_h
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "gmm_vector_matrix.h"

#define MAX_DISTANCE 100
#define MAX_CLUSTERS 5
#define MAX_ERROR 1e-5
#define MIN_MAHALANOBIS_DISTANCE_SQ 1
#define MAX_MAHALANOBIS_SQ_FOR_UPDATE 9
#define SMALL_VALUE_ERROR_OFFSET 1e-4

#define MAX(A,B) (A>B?A:B)

#define ALPHA 0.5
#define BETA 4.

#define UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT 30.
#define SAFE_EXPONENT_MAX_VALUE 2.35385266837019985408e17f

static float safe_exp(double x)
{
    if (x < -UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
        return 0.0f;
    else if (x > UNCALCULABILTY_BOUND_FOR_SAFE_EXPONENT)
        return SAFE_EXPONENT_MAX_VALUE;
    else
        return exp(x);
}

typedef struct
{
    vec2
        mean_in,
        mean_out;
    double
        mahalanobis_sq,
        log_gaussian_norm_factor,
        probability_condition_input,
        score;
    mat2x2
        covariance_in,
        inv_covariance_in,
        covariance_in_out,
        llt_in;
} gaussian_mixture_cluster_t;

typedef struct
{
    gaussian_mixture_cluster_t
       cluster[MAX_CLUSTERS];
    uint8_t
        num_clusters;
    double
        initial_variance;
    vec2
        max_out,
        min_out;
} gaussian_mixture_model_t;

/* Cluster functions */
void InitializeGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, vec2 *, vec2 *, double );
void UpdateGaussianMixtureCluster(                     gaussian_mixture_cluster_t *, vec2 *, vec2 * );
void GetScoreOfGaussianMixtureCluster(                 gaussian_mixture_cluster_t *, vec2 *);
void UpdateNormalOfGaussianMixtureCluster(             gaussian_mixture_cluster_t * );
void UpdateInputProbabilityOfGaussianMixtureCluster(   gaussian_mixture_cluster_t *, double );
void ContributeToOutputOfGaussianMixtureCluster(       gaussian_mixture_cluster_t *, vec2 *, vec2 * );

/* Model functions */
double GetScoreSumOfClustersInGaussianMixtureModel(    gaussian_mixture_model_t *, vec2 * );
double GetOutputAndBestDistanceOfGaussianMixtureModel( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
double GetMaxErrorOfGaussianMixtureModel(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
void AddClusterToGaussianMixtureModel(                 gaussian_mixture_model_t *, vec2 *, vec2 * );
void UpdateGaussianMixtureModel(                       gaussian_mixture_model_t *, vec2 *, vec2 * );
void AddValueToGaussianMixtureModel(                   gaussian_mixture_model_t *, vec2 *, vec2 * );

typedef struct
{
    void (*Initialize)(                 gaussian_mixture_cluster_t *, vec2 *, vec2 *, double );
    void (*Update)(                     gaussian_mixture_cluster_t *, vec2 *, vec2 * );
    void (*GetScore)(                   gaussian_mixture_cluster_t *, vec2 *);
    void (*UpdateNormal)(               gaussian_mixture_cluster_t * );
    void (*UpdateInputProbability)(     gaussian_mixture_cluster_t *, double );
    void (*ContributeToOutput)(         gaussian_mixture_cluster_t *, vec2 *, vec2 * );
} gaussian_mixture_cluster_functions;

typedef struct
{
    double (*GetScoreSumOfClusters)(    gaussian_mixture_model_t *, vec2 * );
    double (*GetOutputAndBestDistance)( gaussian_mixture_model_t *, double, vec2 *, vec2 * );
    double (*GetMaxError)(              gaussian_mixture_model_t *, vec2 *, vec2 *, vec2 * );
    void   (*AddCluster)(               gaussian_mixture_model_t *, vec2 *, vec2 * );
    void   (*Update)(                   gaussian_mixture_model_t *, vec2 *, vec2 * );
    void   (*AddValue)(                 gaussian_mixture_model_t *, vec2 *, vec2 * );
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
    
    .Model.GetScoreSumOfClusters    = GetScoreSumOfClustersInGaussianMixtureModel,
    .Model.GetOutputAndBestDistance = GetOutputAndBestDistanceOfGaussianMixtureModel,
    .Model.GetMaxError              = GetMaxErrorOfGaussianMixtureModel,
    .Model.AddCluster               = AddClusterToGaussianMixtureModel,
    .Model.Update                   = UpdateGaussianMixtureModel,
    .Model.AddValue                 = AddValueToGaussianMixtureModel,
};

#endif /* gmm_h */
