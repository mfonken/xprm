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
    uint32_t    a;
    uint32_t    b;
    uint32_t    c;
};

/* Simple 3d Vector */
struct vec3
{
    uint32_t    ihat;
    uint32_t    jhat;
    uint32_t    khat;
};

/* Transformation Function */
struct vec3 * zxyTransform( struct vec3 * x,
                            struct ang3 * rot,
                            uint8_t reverse)
{
    /* Extract angles */
    uitn32_t a = rot->a;
    uitn32_t b = rot->b;
    uitn32_t c = rot->c;
    if( reverse )
    {
        a *= -1;
        b *= -1;
        c *= -1;
    }
    
    /* Transformation Matrix */
    uint32_t A[3][3];
    A[0][0] = ( cos( a ) * cos( c ) ) - ( sin( a ) * sin( b ) * sin( c ) );
    A[0][1] = - cos( b ) * cos( a );
    A[0][2] = ( cos( b ) * sin( c ) ) + ( cos( c ) * sin( a ) * sin( c ) );
    A[1][0] = ( cos( c ) * sin( a ) ) + ( cos( a ) * sin( b ) * sin( c ) );
    A[1][1] =   cos( a ) * cos( b );
    A[1][2] = ( sin( a ) * sin( c ) ) - ( cos( a ) * cos( c ) * sin( b ) );
    A[2][0] = - cos( b ) * sin( c );
    A[2][1] =   sin( b );
    A[2][2] =   cos( b ) * cos( c );
    
    /* Transformed Vector */
    uint32_t y[3];
    for( uint8_t i = 0; i < 3; i++ )
    {
        y[i] = ( A[i][0] * x[0] ) + ( A[i][1] * x[1] ) + ( A[i][2] * x[2] );
    }
    return &y;
}

void subtractVec3s( struct vec3 * x,
                    struct vec3 * y )
{
    x->a = x->a - y->a;
    x->b = x->b - y->b;
    x->c = x->c - y->c;
}

uint32_t lengthOfVec3( struct vec3 * vec )
{
    return sqrt( ( vec->ihat * vec->ihat ) + ( vec->jhat * vev->jhat ) + ( vec->khat * vec->khat ) );
}

void normalizeVec3( struct vec3 * vec )
{
    uint32 length = lengthOfVec3( vec );
    vec->ihat /= length;
    vec->jhat /= length;
    vec->khat /= length;
}

#endif /* tm_h */
