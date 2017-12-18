/***************************************************************************************************
 * \file   quaternion.h
 * \brief  Quaternion Math Header
 ***************************************************************************************************
 *      Author: Matthew Fonken
 **************************************************************************************************/

#ifndef quaternion_h
#define quaternion_h

#ifdef __cplusplus
extern "C" {
#endif
    
    /* Standard headers */
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
    
    /* Included types header */
#include "matrix.h"
    
    /*! Quaternion hypercomplex type */
    typedef struct
    {
        mfloat x;
        mfloat y;
        mfloat z;
        mfloat w;
    } quaternion_t;
    
    void Euler_To_Quaternion( ang3_t * ang, quaternion_t * quat );
    void Quaternion_To_Euler( quaternion_t * quat, ang3_t * ang );
    void Quaternion_To_Matrix(quaternion_t * quat, mat3x3_t * m);
    void Quaternion_Combine(quaternion_t * a, quaternion_t * b, quaternion_t * c );
    void Rotate_Vector_By_Quaternion(vec3_t * v, quaternion_t * q, vec3_t * r);
    
#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
