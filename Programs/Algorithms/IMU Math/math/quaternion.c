/***********************************************************************************************//**
* \file   matrix.c
* \brief  Matrix Math
***************************************************************************************************
*      Author: Matthew Fonken
**************************************************************************************************/

/* Own header */
#include "quaternion.h"

/* See - http://www.gamasutra.com/view/feature/131686/rotating_objects_using_quaternions.php */
/* Generic intrinsic euler angles to quaternion conversion */
void Euler_To_Quaternion( ang3_t * ang, quaternion_t * quat )
{
    double half_roll, half_pitch, half_yaw;
    half_roll   = ang->x / 2;
    half_pitch  = ang->y / 2;
    half_yaw    = ang->z / 2;
    double cr, cp, cy, sr, sp, sy, cpcy, spsy, spcy, cpsy;
    cr = cos( half_roll  );
    cp = cos( half_pitch );
    cy = cos( half_yaw   );
    sr = sin( half_roll  );
    sp = sin( half_pitch );
    sy = sin( half_yaw   );
    cpcy = cp * cy;
    spsy = sp * sy;
    spcy = sp * cy;
    cpsy = cp * sy;
    quat->w = cr * cpcy + sr * spsy;
    quat->x = sr * cpcy - cr * spsy;
    quat->y = cr * spcy + sr * cpsy;
    quat->z = cr * cpsy - sr * spcy;
}

void Quaternion_To_Euler( quaternion_t * quat, ang3_t * ang )
{
    long double w = quat->w, x = quat->x, y = quat->y, z = quat->z, a, b;
    
    a = 2*(w*x+y*z);
    b = 1-2*(x*x+y*y);
    ang->x = atan2(a, b);
    
    a = 2*(w*y-x*z);
    ang->y = RASIN(a);
    
    a = 2*(w*z+x*y);
    b = 1-2*(y*y+z*z);
    ang->z = atan2(a, b);
}

void Quaternion_To_Matrix(quaternion_t * quat, mat3x3_t * m)
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    
    x2 = quat->x + quat->x; y2 = quat->y + quat->y;
    z2 = quat->z + quat->z;
    xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
    yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
    wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;
    
    m->ii = 1.0 - (yy + zz);
    m->ji = xy - wz;
    m->ki = xz + wy;
    
    m->ij = xy + wz;
    m->jj = 1.0 - (xx + zz);
    m->kj = yz - wx;
    
    m->ik = xz - wy;
    m->jk = yz + wx;
    m->kk = 1.0 - (xx + yy);
}

/* See - https://blog.molecular-matters.com/2013/05/24/a-faster-quaternion-vector-multiplication/ */
void Rotate_Vector_By_Quaternion(vec3_t * v, quaternion_t * q, vec3_t * r)
{
    vec3_t u;
    u.i = q->x;
    u.j = q->y;
    u.k = q->z;
    double s = q->w;
    vec3_t a, b, c, t;
    
    mul3( 2, &u, &a );
    cross3( &a, v, &t );
    cross3( &u, &t, &b );
    mul3( s, &t, &c );
    add33( v, &b, &c, r );
}

/* Double quaternion Hamilton multiplication (Generic) */
void Quaternion_Combine(quaternion_t * a, quaternion_t * b, quaternion_t * c)
{
    double A, B, C, D, E, F, G, H;
    A = ( a->w + a->x ) * ( b->w + b->x );
    B = ( a->z - a->y ) * ( b->y - b->z );
    C = ( a->w - a->x ) * ( b->y + b->z );
    D = ( a->y + a->z ) * ( b->w - b->x );
    E = ( a->x + a->z ) * ( b->x + b->y );
    F = ( a->x - a->z ) * ( b->x - b->y );
    G = ( a->w + a->y ) * ( b->w - b->z );
    H = ( a->w - a->y ) * ( b->w + b->z );
    c->w = B + ( -E - F + G + H ) / 2;
    c->x = A - (  E + F + G + H ) / 2;
    c->y = C + (  E - F + G - H ) / 2;
    c->z = D + (  E - F - G + H ) / 2;
}
/** @} (end addtogroup kinetic) */
/** @} (end addtogroup Application) */
