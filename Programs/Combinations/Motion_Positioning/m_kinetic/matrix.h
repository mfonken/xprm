/*! \file matrix.h
\brief Matrix Math

Created by Matthew Fonken on 10/10/16.
*/

#ifndef matrix_h
#define matrix_h

/*! Tait-Bryan Angles */
typedef struct _ang3_t
{
    double    a;
    double    b;
    double    c;
}ang3_t;

/*! Simple 3d Vector */
typedef struct _vec3_t
{
    double    ihat;
    double    jhat;
    double    khat;
}vec3_t;

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
vec3_t * zxyTransform(  vec3_t *x,
                        ang3_t *rot,
                        bool reverse)
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
    vec3_t *yvec;
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
vec3_t * yxzTransform( vec3_t * x,
                       ang3_t * rot,
                       bool reverse)
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
    vec3_t *yvec;
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
void subtractvec3_ts( vec3_t * x,
                    vec3_t * y )
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
double lengthOfvec3_t( vec3_t * vec )
{
    double i_2 = vec->ihat * vec->ihat;
    double j_2 = vec->jhat * vec->jhat;
    double k_2 = vec->khat * vec->khat;
    return sqrt( i_2 + j_2 + k_2 );
}

/*! Normal of a 3D Vector\r\n
 \f{eqnarray*}{
    \mathbf{v_{norm}} = \frac{\mathbf{v}}{||\mathbf{v}||}
 \f}
 */
void normalizevec3_t( vec3_t * vec )
{
    double length = lengthOfvec3_t( vec );
    vec->ihat /= length;
    vec->jhat /= length;
    vec->khat /= length;
}

#endif /* matrix_h */
