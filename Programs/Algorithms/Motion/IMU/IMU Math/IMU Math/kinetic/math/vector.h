/***********************************************************************************************//**
* \file   matrix.h
* \brief  Matrix Math Header
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

#ifndef vector_h
#define vector_h

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Standard headers */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
    
#include "matrix.h"
#include "math_types.h"
    
    /*! Simple 3d Vector */
    typedef struct _vec3_t
    {
        double    i;             /**< \$f\hat{i}\$f value */
        double    j;             /**< \$f\hat{j}\$f value */
        double    k;             /**< \$f\hat{k}\$f value */
    } vec3_t;
    
    struct vector
    {
        double  (*len3)(   vec3_t * v );
        void    (*nrm3)(   vec3_t * v );
        void    (*sub3)(   vec3_t * x, vec3_t * y );
        double  (*dot3)(   vec3_t * u, vec3_t * v );
        double  (*ang3)(   vec3_t * u, vec3_t * v );
        void    (*mul3)(   double m,   vec3_t * v, vec3_t * r );
        void    (*cross3)( vec3_t * u, vec3_t * v, vec3_t * r );
        void    (*add33)(  vec3_t * u, vec3_t * v, vec3_t * w, vec3_t * r);
    };
    
    extern const struct vector Vector;
    
#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
