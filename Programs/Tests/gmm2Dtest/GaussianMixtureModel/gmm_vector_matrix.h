//
//  gmm_vector_matrix.h
//  GaussianMixtureModel
//
//  Created by Matthew Fonken on 2/10/19.
//  Copyright © 2019 Matthew Fonken. All rights reserved.
//

#ifndef gmm_vector_matrix_h
#define gmm_vector_matrix_h

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
static void getMat2x2Inverse( mat2x2 * mat, mat2x2 * res )
{
    double det = getMat2x2Determinant( mat );
    res->a = mat->a / det;
    res->b = mat->b / det;
    res->c = mat->c / det;
    res->d = mat->d / det;
}
static double * GetMat2x2ValuePointerByIndex(mat2x2 * mat, uint8_t a, uint8_t b)
{
    uint8_t offset = a + ( b << 1 );
    return (double *)(mat+offset);
}

static void mat2x2DotVec2(mat2x2 * mat, vec2 * vec, vec2 * res)
{
    res->a = mat->a * vec->a + mat->b * vec->b;
    res->a = mat->c * vec->a + mat->d * vec->b;
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
    mat2x2 L = { 0., 0., 0., 0. };
    L.a = sqrt( mat->a );
    L.c = mat->c / L.a;
    L.d = sqrt( mat->d - L.c );
    mat2x2 LT = { L.a, L.c, L.b, L.d };
    mat2x2MulMat2x2( &L, &LT, llt );
}

static double CalculateMahalanobisDistanceSquared(mat2x2 * inv_covariance, vec2 * delta)
{
    vec2 inv_covariance_delta;
    mat2x2DotVec2(inv_covariance, delta, &inv_covariance_delta);
    double mahalanobis_distance_squared;
    vec2DotVec2(delta, &inv_covariance_delta, & mahalanobis_distance_squared);
    return mahalanobis_distance_squared;
}



#endif /* gmm_vector_matrix_h */
