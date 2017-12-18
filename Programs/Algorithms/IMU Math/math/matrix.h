/***********************************************************************************************//**
* \file   matrix.h
* \brief  Matrix Math Header
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

#ifndef matrix_h
#define matrix_h

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Standard headers */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
    
    /* Included types header */
#include "vector.h"
    
    void Multiply_Vec_3x1( mat3x3_t * a, vec3_t * b, vec3_t * c );
    void sub3( vec3_t * x, vec3_t * y );
    double len3( vec3_t * vec );
    double len2( vec2_t * vec );
    void nrm3( vec3_t * vec );
    double dist2( cartesian2_t *a, cartesian2_t *b );
    double ang3( vec3_t * u, vec3_t * v);
    double dot3( vec3_t * u, vec3_t * v );
    void cross3( vec3_t * u, vec3_t * v, vec3_t * r );
    void mul3( double m, vec3_t * v, vec3_t * r );
    void add33( vec3_t * u, vec3_t * v, vec3_t * w, vec3_t * r);
    void ang3Deg_To_Rad( ang3_t * a );
    void ang3Rad_To_Deg( ang3_t * a );
    
#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
