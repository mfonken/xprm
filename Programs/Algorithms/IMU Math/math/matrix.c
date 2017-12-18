/***********************************************************************************************//**
* \file   matrix.c
* \brief  Matrix Math
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

/* Own header */
#include "matrix.h"

void Multiply_Vec_3x1( mat3x3_t * a, vec3_t * b, vec3_t * c )
{
    c->i = a->ii * b->i + a->ij * b->j + a->ik * b->k;
    c->j = a->ji * b->i + a->jj * b->j + a->jk * b->k;
    c->k = a->ki * b->i + a->kj * b->j + a->kk * b->k;
}

/* Generic vector3 manipulations */
double dist2( cartesian2_t *a, cartesian2_t *b )
{
    double a_x = a->x;
    double a_y = a->y;
    double b_x = b->x;
    double b_y = b->y;
    
    return sqrt( ( ( b_x * b_x ) - ( a_x * a_x ) ) + ( ( b_y * b_y ) - ( a_y * a_y ) ) );
}

void sub3( vec3_t * x,
          vec3_t * y )
{
    x->i = x->i - y->i;
    x->j = x->j - y->j;
    x->k = x->k - y->k;
}

double len3( vec3_t * vec )
{
    double i_2 = vec->i * vec->i;
    double j_2 = vec->j * vec->j;
    double k_2 = vec->k * vec->k;
    return sqrt( i_2 + j_2 + k_2 );
}

double len2( vec2_t * vec )
{
    double i_2 = vec->i * vec->i;
    double j_2 = vec->j * vec->j;
    return sqrt( i_2 + j_2 );
}

void norm3( vec3_t * vec )
{
    double length = len3( vec );
    if(!length) return;
    vec->i /= length;
    vec->j /= length;
    vec->k /= length;
}

/* Angle between two vectors */
double ang3( vec3_t * u, vec3_t * v)
{
    return acos( ZDIV( dot3( u, v ), ( len3(u) * len3(v) ) ) );
}

/* Dot product of two vectors */
double dot3( vec3_t * u, vec3_t * v )
{
    return u->i*v->i + u->j*v->j + u->k*v->k;
}
/* Cross product of two vectors */
void cross3( vec3_t * u, vec3_t * v, vec3_t * r )
{
    r->i = u->j*v->k - u->k*v->j;
    r->j = u->k*v->i - u->i*v->k;
    r->k = u->i*v->j - u->j*v->i;
}

/* Multiply a vector by a scalar */
void mul3( double m, vec3_t * v, vec3_t * r )
{
    r->i = m * v->i;
    r->j = m * v->j;
    r->k = m * v->k;
}

/* Add three vectors */
void add33( vec3_t * u, vec3_t * v, vec3_t * w, vec3_t * r)
{
    r->i = u->i + v->i + w->i;
    r->j = u->j + v->j + w->j;
    r->k = u->k + v->k + w->k;
}

void ang3Rad_To_Deg( ang3_t * a )
{
    a->x *= RAD_TO_DEG;
    a->y *= RAD_TO_DEG;
    a->z *= RAD_TO_DEG;
}

void ang3Deg_To_Rad( ang3_t * a )
{
    a->x *= DEG_TO_RAD;
    a->y *= DEG_TO_RAD;
    a->z *= DEG_TO_RAD;
}

/** @} (end addtogroup kinetic) */
/** @} (end addtogroup Application) */
