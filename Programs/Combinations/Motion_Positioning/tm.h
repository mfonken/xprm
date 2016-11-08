/*! \file tm.h
\brief Transformation Matrix

Created by Matthew Fonken on 10/10/16.
*/

#ifndef tm_h
#define tm_h

/* Tait-Bryan Angles */
struct typedef _ang3
{
    double    a;
    double    b;
    double    c;
}ang3;

/* Simple 3d Vector */
struct typedef _vec3
{
    double    ihat;
    double    jhat;
    double    khat;
}vec3;

struct typedef _cartesian2
{
    double x;
    double y;
}cartesion2;

/*! Tait-Bryan ZXY Transformation Function\r\n
 \f{eqnarray*}{
     &\mathbf{A} = \left[
         \begin{matrix}
         \cos(a)\cos(c) - \sin(a)\sin(b)\sin(c) & -\cos(b)\sin(a) & \cos(a)\sin(c) + \cos(c)\sin(a)\sin(b)\\
         \cos(c)sin(a) + cos(a)\sin(b)\sin(c) & \cos(a)\cos(b) & \sin(a)\sin(c) - \cos(a)\cos(c)\sin(b) \\
         -\cos(b)\sin(c) & \sin(b) & \cos(b)\cos(c)\\
         \end{matrix}
         \right] \\
     &\mathbf{y} = \mathbf{A}\mathbf{x}
 \f}
 */
vec3 * zxyTransform( vec3 * x,
                     ang3 * rot,
                     uint8_t reverse)
{
    /* Extract angles */
    double a = rot->a;
    double b = rot->b;
    double c = rot->c;
    if( reverse )
    {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    
    /* Transformation Matrix */
    double A[3][3];
    A[0][0] = ( cos( a ) * cos( c ) ) - ( sin( a ) * sin( b ) * sin( c ) );
    A[0][1] = - cos( b ) * sin( a );
    A[0][2] = ( cos( a ) * sin( c ) ) + ( cos( c ) * sin( a ) * sin( b ) );
    A[1][0] = ( cos( c ) * sin( a ) ) + ( cos( a ) * sin( b ) * sin( c ) );
    A[1][1] =   cos( a ) * cos( b );
    A[1][2] = ( sin( a ) * sin( c ) ) - ( cos( a ) * cos( c ) * sin( b ) );
    A[2][0] = - cos( b ) * sin( c );
    A[2][1] =   sin( b );
    A[2][2] =   cos( b ) * cos( c );
    
    /* Transformed Vector */
    double y[3];
    for( uint8_t i = 0; i < 3; i++ )
    {
        y[i] = ( A[i][0] * x->ihat ) + ( A[i][1] * x->jhat ) + ( A[i][2] * x->khat );
    }
    struct vec3 *yvec;
    yvec->ihat = y[0];
    yvec->jhat = y[1];
    yvec->khat = y[2];
    return yvec;
}


/*! Tait-Bryan YXZ Transformation Function\r\n
 \f{eqnarray*}{
    &\mathbf{A} = \left[
     \begin{matrix}
        \cos(a)\cos(c) + \sin(a)\sin(b)\sin(c) & \cos(c)\sin(a)\sin(b) - \cos(a)\sin(c) & \cos(b)\sin(a) \\
        \cos(b)\sin(c) & \cos(b)\cos(c) & -\sin(b) \\
        \cos(a)sin(b)sin(c) - \cos(c)\sin(a) & \cos(a)\cos(c)\cos(b) + \sin(a)\sin(c) & \cos(a)\cos(b) \\
     \end{matrix}
    \right] \\
    &\mathbf{y} = \mathbf{A}\mathbf{x}
 \f}
 */
vec3 * yxzTransform( vec3 * x,
                     ang3 * rot,
                     uint8_t reverse)
{
    /* Extract angles */
    double a = rot->a;
    double b = rot->b;
    double c = rot->c;
    if( reverse )
    {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    
    /* Transformation Matrix */
    double A[3][3];
    A[0][0] = ( cos( a ) * cos( c ) ) + ( sin( a ) * sin( b ) * sin( c ) );
    A[0][1] = ( cos( c ) * sin( a ) * sin( b ) ) - ( cos( a ) * sin( c ) );
    A[0][2] =   cos( b ) * sin( a );
    A[1][0] =   cos( b ) * sin( c );
    A[1][1] =   cos( b ) * cos( c );
    A[1][2] = - sin( b );
    A[2][0] = ( cos( a ) * sin( b ) * sin( c ) ) - ( cos( c ) * sin( a ) );
    A[2][1] = ( cos( a ) * cos( c ) * cos( b ) ) + ( sin( a ) * sin( c ) );
    A[2][2] =   cos( a ) * cos( b );
    
    /* Transformed Vector */
    double y[3];
    for( uint8_t i = 0; i < 3; i++ )
    {
        y[i] = ( A[i][0] * x->ihat ) + ( A[i][1] * x->jhat ) + ( A[i][2] * x->khat );
    }
    struct vec3 *yvec;
    yvec->ihat = y[0];
    yvec->jhat = y[1];
    yvec->khat = y[2];
    return yvec;
}

/*! Difference of 3D Vectors\r\n
 \f{eqnarray*}{
    &x_\hat{i} = x_\hat{i} - y_\hat{i} \\
    &x_\hat{j} = x_\hat{j} - y_\hat{j} \\
    &x_\hat{k} = x_\hat{k} - y_\hat{k}
 \f}
 */
void subtractVec3s( vec3 * x,
                    vec3 * y )
{
    x->ihat = x->ihat - y->ihat;
    x->jhat = x->jhat - y->jhat;
    x->khat = x->khat - y->khat;
}

/*! Length of a 3D Vector\r\n
 \f{eqnarray*}{
    ||\mathbf{v}|| = \sqrt{v_\hat{i}^2 + v_\hat{j}^2 + v_\hat{k}^2}\\
 \f}
 */
double lengthOfVec3( vec3 * vec )
{
    return sqrt( ( vec->ihat * vec->ihat ) + ( vec->jhat * vec->jhat ) + ( vec->khat * vec->khat ) );
}

/*! Normal of a 3D Vector\r\n
 \f{eqnarray*}{
    \mathbf{v_{norm}} = \frac{\mathbf{v}}{||\mathbf{v}||}
 \f}
 */
void normalizeVec3( vec3 * vec )
{
    double length = lengthOfVec3( vec );
    vec->ihat /= length;
    vec->jhat /= length;
    vec->khat /= length;
}

#endif /* tm_h */
