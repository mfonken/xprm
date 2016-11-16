/*! \file cartesian.h
    \brief Cartesian 2D
 */

#ifndef cartesian_h
#define cartesian_h

typedef struct _cartesian2_t
{
    double x;
    double y;
}cartesion2_t;

/*! Distance between to Cartesian coordinates
 \f{eqnarray*}{
 d = \sqrt{(x_b^2 - x_a^2) + (y_b^2 - y_a^2)}
 \f}
 */
uint32_t get2dDistance( cartesian2_t *a, cartesian2_t *b )
{
    uint32_t a_x = a->x;
    uint32_t a_y = a->y;
    uint32_t b_x = b->x;
    uint32_t b_y = b->y;
    
    return sqrt( ( ( b_x * b_x ) - ( a_x * a_x ) ) + ( ( b_y * b_y ) - ( a_y * a_y ) ) );
}


#endif /* cartesian_h */
