//
//  tm.h
//
//  Transformation Matrix
//
//  Created by Matthew Fonken on 10/10/16.
//
//

#ifndef tm_h
#define tm_h

/* Tait-Bryan Angles */
struct ang3
{
    double    a;
    double    b;
    double    c;
};

/* Simple 3d Vector */
struct vec3
{
    double    ihat;
    double    jhat;
    double    khat;
};

struct cartesian2
{
    double x;
    double y;
};

/* Transformation Function - Tait Bryan ZXY */
struct vec3 * zxyTransform( struct vec3 * x,
                            struct ang3 * rot,
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

/* Transformation Function - Tait Bryan YXZ */
struct vec3 * yxzTransform( struct vec3 * x,
                            struct ang3 * rot,
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

void subtractVec3s( struct vec3 * x,
                    struct vec3 * y )
{
    x->ihat = x->ihat - y->ihat;
    x->jhat = x->jhat - y->jhat;
    x->khat = x->khat - y->khat;
}

double lengthOfVec3( struct vec3 * vec )
{
    return sqrt( ( vec->ihat * vec->ihat ) + ( vec->jhat * vec->jhat ) + ( vec->khat * vec->khat ) );
}

void normalizeVec3( struct vec3 * vec )
{
    double length = lengthOfVec3( vec );
    vec->ihat /= length;
    vec->jhat /= length;
    vec->khat /= length;
}

#endif /* tm_h */
