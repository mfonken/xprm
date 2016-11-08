//
//
//  
//
//
//
//

/*! \file sin_cos_atan.h
 \brief Trig shortcut for fisheye.
 
 Created by Matthew Fonken on 11/2/16.
 */

#ifndef sin_cos_atan_h
#define sin_cos_atan_h

#include "types.h"

/*! s_c_a - Sine/Cosine of Arctan\r\n
 
 \f{eqnarray*}{
    *s &=& \frac{z}{\sqrt{z^2+1}} &=& \sin(\tan^{-1}(z)) \\
    *c &=& \frac{1}{\sqrt{z^2+1}} &=& \cos(\tan^{-1}(z))
 \f}
 */
void s_c_a( coord * z, double * s, double * c )
{
    double x = ( *z ).x;
    double y = ( *z ).y;
    
    if( x == 0 )
    {
                            *c =  0;
        if( y >= 0 )        *s =  1;
        else if( y < 0 )    *s = -1;
        return;
    }
    
    double a        = y / x;
    int    p        = x > 0 ? 1 : -1;
    double d        = sqrt( ( a * a ) + 1 );
    
    *c              = p * ( 1 / d );
    *s              = p * ( a / d );
}

#endif /* sin_cos_atan_h */
