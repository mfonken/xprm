//
//  control_type.h
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef control_types_h
#define control_types_h

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

    
#ifndef ZDIV
#define ZDIV_LNUM 1 << 16
#define ZDIV(X,Y) ((Y==0)?(X==0?0:ZDIV_LNUM):X/Y)
#endif
    
#define MAX_INPUT_COVARIANCE  300
    
#define IN_RANGE(A,X,Y) ( ( A > X ) && ( A < Y ) )
#ifndef MIN
#define MIN(A,B) (A<B?A:B)
#endif
#ifndef MAX
#define MAX(A,B) (A>B?A:B)
#endif  
    
#define ALPHA 0.025//0.1
#define BETA 1//4.
    
#define MAX_LABELS 10
#define LABEL_MOVING_AVERAGE_MAX_HISTORY 10
#define NULL_LABEL 0xff
#define MIN_LABEL_CONTRIBUTION 0.2
    
#define BOUNDARY_START(X)   !!(X<0)
#define BOUNDARY_END(X)     !!(X>0)
    
#define NUM_STATE_GROUPS 4
    
#define MAX_THRESH 255
    
#define M_C 1.f
#define M_1_C (1.0f / M_C)
    
    //#define NUM_STATES              10
#define NUM_OBSERVATION_SYMBOLS 5 // Max number of clusters in GMM
#define MAX_OBSERVATIONS        (1 << 7) // Length of history
#define MAX_OBSERVATION_MASK    (MAX_OBSERVATIONS-1)
    
#define MAX_STD_DEVS_TO_BE_INCLUDED_IN_BAND_CALC 3
#define MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC 10
#define MIN_VARIANCE_SPAN_TO_REJECT_FOR_BAND_CALC ( MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC * MIN_STD_DEV_SPAN_TO_REJECT_FOR_BAND_CALC )
    
#define MAX_DISTANCE 1000.f
#define MIN_TOTAL_MIXTURE_PROBABILITY 1e-15f
#define ABSOLUTE_MAX_CLUSTERS 300
#define MAX_CLUSTERS 100
#define MAX_ERROR 0.2
#define INITIAL_VARIANCE 57//100//150//3//3
#define INV_INITIAL_VARIANCE (1./INITIAL_VARIANCE)
#define MAX_MAHALANOBIS_SQ 9//.386f
#define MAX_MAHALANOBIS_SQ_FOR_UPDATE MAX_MAHALANOBIS_SQ//20.f
#define SMALL_VALUE_ERROR_OFFSET 1e-4f
#define VALID_CLUSTER_STD_DEV 0.25
#define MIN_CLUSTER_SCORE 0.05///1e-3f
#define FALLBACK_MAX_ERROR 1e-2f
    
typedef struct
{
    double
    max_distance,
    max_error,
    initial_variance,
    max_mahalanobis_sq,
    small_value_error_offset,
    min_cluster_score,
    fallback_max_error,
    max_clusters,
    alpha,
    beta,
    inv_initial_variance,
    max_mahalanobis_sq_for_update;
} gmm_settings_t;
    
    #define NUM_SETTINGS_TO_SKIP 2
    
    static char gmm_settings_names[sizeof(gmm_settings_t)/sizeof(double)-NUM_SETTINGS_TO_SKIP][50] =
    {
        "Max Distance",
        "Max Error",
        "Initial Variance",
        "Max Mah.^2 Distance ",
        "Small Error Offset",
        "Min Cluster Score",
        "Fallback Offset",
        "Max Clusters",
        "Alpha",
        "Beta"
    };

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
        double a,b,c,d;
    } mat2x2;
    
    typedef struct
    {
        double a,b;
    } vec2;
    
    static void vec2SubVec2(vec2 * A, vec2 * B, vec2 * C)
    {
        C->a = A->a - B->a;
        C->b = A->b - B->b;
    }
    static void vec2AddVec2(vec2 * A, vec2 * B, vec2 * C)
    {
        C->a = A->a + B->a;
        C->b = A->b + B->b;
    }
    static void scalarMulVec2(double A, vec2 * B, vec2 * C)
    {
        C->a = A * B->a;
        C->b = A * B->b;
    }
    static double getMat2x2Determinant( mat2x2 * mat )
    {
        return ( ( mat->a * mat->d ) - ( mat->c * mat->b ) );
    }
    static void transposeMat2x2( mat2x2 * mat )
    {
        double temp = mat->b;
        mat->b = mat->c;
        mat->c = temp;
    }
    static void getMat2x2Inverse( mat2x2 * mat, mat2x2 * res )
    {
        double det = getMat2x2Determinant( mat );
        if( det )
        {
            res->d = mat->a / det;
            res->c = -mat->b / det;
            res->b = -mat->c / det;
            res->a = mat->d / det;
        }
        else
            memset( res, ZDIV_LNUM, sizeof(mat2x2) );
    }
    static double * GetMat2x2ValuePointerByIndex(mat2x2 * mat, uint8_t a, uint8_t b)
    {
        uint8_t offset = a + ( b << 1 );
        return (double *)(mat+offset);
    }
    
    static void mat2x2DotVec2(mat2x2 * mat, vec2 * vec, vec2 * res)
    {
        res->a = mat->a * vec->a + mat->b * vec->b;
        res->b = mat->c * vec->a + mat->d * vec->b;
    }
    static void mat2x2AddMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * C)
    {
        C->a = A->a + B->a;
        C->b = A->b + B->b;
        C->c = A->c + B->c;
        C->d = A->d + B->d;
    }
    static void mat2x2SubMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * C)
    {
        C->a = A->a - B->a;
        C->b = A->b - B->b;
        C->c = A->c - B->c;
        C->d = A->d - B->d;
    }
    static void scalarMulMat2x2( double A, mat2x2 * B, mat2x2 * C )
    {
        C->a = A * B->a;
        C->b = A * B->b;
        C->c = A * B->c;
        C->d = A * B->d;
    }
    static void powerMulMat2x2( double A, mat2x2 * B, mat2x2 * C )
    {
        C->a = pow( B->a, A );
        C->b = pow( B->b, A );
        C->c = pow( B->c, A );
        C->d = pow( B->d, A );
    }
    static void mat2x2MulMat2x2(mat2x2 * A, mat2x2 * B, mat2x2 * res)
    {
        res->a = A->a * B->a + A->b * B->c;
        res->b = A->a * B->b + A->b * B->d;
        res->c = A->c * B->a + A->d * B->c;
        res->d = A->c * B->b + A->d * B->d;
    }
    
    static void vec2DotVec2(vec2 * A, vec2 * B, double * res)
    {
        *res = A->a * B->a + A->b * B->b;
    }
    
    static void GetMat2x2LLT( mat2x2 * mat, mat2x2 * llt )
    {
        //printf("m_llt: [%.2f %.2f | %.2f %.2f] ", mat->a, mat->b, mat->c, mat->d);
        mat2x2 L = { 0., 0., 0., 0. };
        L.a = sqrt( mat->a );
        L.b = mat->b;
        L.c = ZDIV( mat->c, L.a );
        L.d = sqrt( mat->d - L.c * L.c );
        if( L.d == 0) L.d = 1;
        //printf("[%.2f %.2f | %.2f %.2f]\n", L.a, L.b, L.c, L.d);
        *llt = L;
    }
    
    static double CalculateMahalanobisDistanceSquared(mat2x2 * inv_covariance, vec2 * delta)
    {
        vec2 inv_covariance_delta;
        mat2x2DotVec2(inv_covariance, delta, &inv_covariance_delta);
        double mahalanobis_distance_squared;
        vec2DotVec2(delta, &inv_covariance_delta, &mahalanobis_distance_squared);
        return mahalanobis_distance_squared;
    }
    
    typedef struct
    {
        double
        mean,
        std_dev;
    } gaussian1d_t;
    
    typedef struct
    {
        vec2 mean;
        mat2x2 covariance;
        uint8_t combinations;
    } gaussian2d_t;
    
    static gaussian1d_t getGaussian1dFrom2dY( gaussian2d_t * a )
    {
        return (gaussian1d_t){ a->mean.b, a->covariance.d };
    }
    
    static void mulGaussian1d( gaussian1d_t * a, gaussian1d_t * b, gaussian1d_t * c )
    {
        double cov_y_a_var = a->std_dev * a->std_dev, cov_y_b_var = b->std_dev * b->std_dev;
        double k = cov_y_a_var / ( cov_y_a_var + cov_y_b_var), mean_diff = b->mean - a->mean;
        c->mean = a->mean + k * mean_diff;
        c->std_dev = a->std_dev * ( 1 - k );
    }
    static void mulGaussian2d( gaussian2d_t * a, gaussian2d_t * b, gaussian2d_t * c )
    { /* See https://math.stackexchange.com/questions/157172/product-of-two-multivariate-gaussians-distributions */
        mat2x2 a_computation, b_computation, c_computation;
        
        getMat2x2Inverse( &a->covariance, &a_computation );
        getMat2x2Inverse( &b->covariance, &b_computation );
        mat2x2AddMat2x2( &a_computation, &b_computation, &c_computation );
        getMat2x2Inverse( &c_computation, &c->covariance );
        
        vec2 a_computation_vec, b_computation_vec, c_computation_vec;
        mat2x2DotVec2( &a_computation, &a->mean, &a_computation_vec );
        mat2x2DotVec2( &b_computation, &b->mean, &b_computation_vec );
        vec2AddVec2( &a_computation_vec, &b_computation_vec, &c_computation_vec );
        mat2x2DotVec2( &c->covariance, &c_computation_vec, &c->mean );

        c->combinations = a->combinations + b->combinations + 1;
    }
    static void divGaussian1d( gaussian1d_t * a, gaussian1d_t * b, gaussian1d_t * c )
    {
        double cov_y_a_var = a->std_dev * a->std_dev, cov_y_b_var = b->std_dev * b->std_dev;
        double k = cov_y_a_var / ( cov_y_a_var - cov_y_b_var), mean_diff = a->mean - b->mean;
        c->mean = -a->mean + k * mean_diff;
        c->std_dev = a->std_dev * ( 1 + k );
    }
    
    static void copyGaussian1d( gaussian1d_t * a, gaussian1d_t * b )
    {
        b->mean = a->mean;
        b->std_dev = a->std_dev;
    }
    
    typedef struct
    {
        double a, b;
        uint8_t label;
    } observation_t;
    
    typedef struct
    {
        double value;
        int8_t label;
    } cluster_boundary_t;
    
    typedef struct
    {
        uint8_t length;
        cluster_boundary_t list[ABSOLUTE_MAX_CLUSTERS*2];
    } cluster_boundary_list_t;
    
    typedef struct
    {
        double average[MAX_LABELS];
        uint8_t count[MAX_LABELS];
        uint8_t num_valid;
    } label_manager_t;
    
    typedef struct
    {
        double
            lower_boundary,
            upper_boundary;
        vec2
            true_center;
        double variance;
    } band_t;
    
    typedef struct
    {
        uint8_t length;
        band_t band[NUM_STATE_GROUPS];
    } band_list_t;
    
    static double GetMeanXAtGaussianYOffset( gaussian2d_t * gaussian, double y_offset )
    { /* See - https://courses.cs.washington.edu/courses/cse590b/02wi/eig2x2.cpp
       * and http://www.math.harvard.edu/archive/21b_fall_04/exhibits/2dmatrices/index.html
       */
        /* Find maximum eigen value */
        double a = gaussian->covariance.a, b = gaussian->covariance.b, d = gaussian->covariance.d,
        a_minus_d = a - d,
        radius = sqrt( a_minus_d * a_minus_d + 4. * b * b ),
        lambda = a_minus_d + radius;
        return y_offset * ZDIV( -2 * b, a - lambda );
    }
    
    static vec2 WeightedIncreaseMean( vec2 * new_val, gaussian2d_t * gaussian, double weight )
    {
        vec2 delta_mean, weighted_mean;
        vec2SubVec2( new_val, &gaussian->mean, &delta_mean );
        scalarMulVec2( weight, &delta_mean, &weighted_mean );
        vec2AddVec2( &weighted_mean, &gaussian->mean, &gaussian->mean );
        return delta_mean;
    }
    
    static bool LimitCovariance( mat2x2 * covariance )
    {
        bool limited = false;
        double max = covariance->a, check;
        for(uint8_t i = 1; i < 4; i++ )
        {
            check = ((double *)covariance)[i];
            if( check > max )
                max = check;
        }
        if( max > MAX_INPUT_COVARIANCE )
        {
            double correction_factor = MAX_INPUT_COVARIANCE / max;
            for(uint8_t i = 0; i < 4; i++ )
                ((double *)covariance)[i] *= correction_factor;
            limited = true;
        }
        return limited;
    }
    
    static void UpdateCovarianceWithWeight( vec2 * A, vec2 * B, gaussian2d_t * gaussian, double weight, double small_value_error_offset )
    {       
        double delta_mean_a_a = A->a * B->a,
            delta_mean_a_b = A->a * B->b,
            delta_mean_b_b = A->b * B->b;
        if( delta_mean_a_a == 0. ) delta_mean_a_a += small_value_error_offset;
        if( delta_mean_b_b == 0. ) delta_mean_b_b += small_value_error_offset;
        mat2x2 covariance_delta_factor, unweighted_covariance_factor =
        { delta_mean_a_a, delta_mean_a_b, delta_mean_a_b, delta_mean_b_b };
        
        //printf("m_covf: <%.2f %.2f> [%.2f %.2f | %.2f %.2f] | m_weig: %.2f\n", A->a, A->b, unweighted_covariance_factor.a, unweighted_covariance_factor.b, unweighted_covariance_factor.c, unweighted_covariance_factor.d, weight);
        
        //printf("m_cvin: [%.2f %.2f | %.2f %.2f]\n", gaussian->covariance.a, gaussian->covariance.b, gaussian->covariance.c, gaussian->covariance.d);
        
        mat2x2SubMat2x2( &unweighted_covariance_factor, &gaussian->covariance, &covariance_delta_factor );
        scalarMulMat2x2( weight, &covariance_delta_factor, &covariance_delta_factor );
        mat2x2AddMat2x2( &gaussian->covariance, &covariance_delta_factor, &gaussian->covariance );
        
        LimitCovariance( &gaussian->covariance );
    }
    
    static bool ReportLabel( label_manager_t * labels, uint8_t new_label )
    {
        if( new_label > MAX_LABELS ) return false;
        labels->count[new_label]++;
        double sum = 0;
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
            sum += (double)labels->count[i];
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
            labels->average[i] = (double)labels->count[i] / sum;
        return true;
    }
    
    static void GetTopTwoLabels( label_manager_t * labels, uint8_t * first, uint8_t * second )
    {
        *first = 0; *second = 1;
        double first_value = labels->average[0], second_value = labels->average[1], current = 0.;
        for( uint8_t i = 2; i < MAX_LABELS; i++ )
        {
            current = labels->average[i];
            if( current > first_value )
            {
                second_value = first_value;
                *second = *first;
                first_value = current;
                *first = i;
            }
            else if ( current > second_value )
            {
                second_value = current;
                *second = i;
            }
        }
    }
    
    static uint32_t GetValidLabels( label_manager_t * labels )
    {
#if MAX_LABELS > 32
#warning "Valid label mask is max 32 bit wide."
#endif
        uint32_t valid_vector = 0;
        for( uint8_t i = 0; i < MAX_LABELS; i++ )
        {
            valid_vector |= ( labels->average[i] > MIN_LABEL_CONTRIBUTION ) << i;
        }
        return valid_vector;
    }
    static uint8_t CountSet( uint32_t v )
    {
        uint8_t c = 0;
        for( uint8_t i = 0, m = 1; i < 32; i++, c += !!(v & m), m <<= 1 );
        return c;
    }
    
    typedef struct
    {
        uint8_t
        data[MAX_OBSERVATIONS],
        next, first;
    } observation_buffer_t;
    
    static uint8_t addToObservationBuffer( observation_buffer_t * buffer, uint8_t v )
    {
        uint8_t index = buffer->next;
        buffer->data[buffer->next++] = v;
        buffer->next &= MAX_OBSERVATION_MASK;
        if(buffer->next == buffer->first)
            buffer->first = ( buffer->next + 1 ) & MAX_OBSERVATION_MASK;
        return index;
    }
    
    static double NumStdDevsFromYMean( gaussian2d_t * gaussian, double check_value )
    {
        double diff = fabs( gaussian->mean.b - check_value );
        double num_std_devs = ZDIV( diff, sqrt( gaussian->covariance.d ) );
        return num_std_devs;
    }
    
#ifdef __cplusplus
}
#endif

#endif /* control_types_h */
