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
    
    /*! Simple 3d Vector */
    typedef struct _mat3x3_t
    {
        double    ii;               /**< \$f\hat{ii}\$f value */
        double    ji;               /**< \$f\hat{ji}\$f value */
        double    ki;               /**< \$f\hat{ki}\$f value */
        double    ij;               /**< \$f\hat{ij}\$f value */
        double    jj;               /**< \$f\hat{jj}\$f value */
        double    kj;               /**< \$f\hat{kj}\$f value */
        double    ik;               /**< \$f\hat{ik}\$f value */
        double    jk;               /**< \$f\hat{jk}\$f value */
        double    kk;               /**< \$f\hat{kk}\$f value */
    } mat3x3_t;
    
#ifdef __cplusplus
}
#endif

#endif /* matrix_h */
