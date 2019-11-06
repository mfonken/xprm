/***********************************************************************************************//**
 * \file   kinetic.c
 * \brief  Kinetic Motion code
 ***************************************************************************************************
 *  Created on: Nov 13, 2016
 *      Author: Matthew Fonken
 **************************************************************************************************/

/* Own header */
#include "kinetic.h"

/***********************************************************************************************//**
 *  \brief  Initialize Kinetic Sensors
 **************************************************************************************************/
void KineticDefaultInit( kinetic_t * k )
{
    KineticFunctions.Init( k, KINETIC_CAMERA_WIDTH, KINETIC_CAMERA_HEIGHT, FOCAL_LENGTH, D_FIXED );
}

void KineticInit( kinetic_t * k, int W, int H, floating_t F, floating_t L )
{
    k->W = W;
    k->H = H;
    k->f_l = F;
    k->d_l = L;
    Camera_Rotation_Init(k);
    Reference_Rotation_Init(k);

#ifdef __KALMAN__
    Filters_Init(k);
#endif
}

static void KineticUpdateRotation( kinetic_t * k, ang3_t * e, ang3_t * g )
{
#ifdef __KALMAN__
    /* Step 1: Update Pitch (Restricted) */
    floating_t v = k->filters.rotation[0].value;
    if( ( e->x < -M_PI_2 && v >  M_PI_2 ) ||
       ( e->x >  M_PI_2 && v < -M_PI_2 ) )
    {
        k->filters.rotation[0].value = e->x;
        k->values.rotation[0] = e->x;
    }
    else
    {
        Kalman.Step( &k->filters.rotation[0], e->x, g->x );
        k->values.rotation[0] = k->filters.rotation[0].value;
    }
    
    if ( k->values.rotation[0] > M_PI_2 )
    {
        /* Invert rate, so it fits the restricted accelerometer reading */
        g->x = -g->x;
    }
    
    /* Step 2: Update Roll */
    Kalman.Step( &k->filters.rotation[1], e->y, g->y );
    k->values.rotation[1] = k->filters.rotation[1].value;
    
    /* Step 3: Update Yaw */
    Kalman.Step( &k->filters.rotation[2], e->z, g->z );
    k->values.rotation[2] = k->filters.rotation[2].value;
    
    k->values.rotation[0] += REFERENCE_OFFSET_ANGLE_X;
    k->values.rotation[1] += REFERENCE_OFFSET_ANGLE_Y;
    k->values.rotation[2] += REFERENCE_OFFSET_ANGLE_Z;
    
    /* Get proper device angles from kinetic */
    k->e.x = k->values.rotation[0];
    k->e.y = k->values.rotation[1];
    k->e.z = k->values.rotation[2];
    Quaternion.fromEuler( &k->e, &k->qd );
#else
    memcpy( &k->e, e, sizeof(ang3_t) );
#endif
}

static void KineticUpdatePosition( kinetic_t * k, vec3_t * n, kpoint_t A, kpoint_t B )
{
    /* Step 1: Calculate Minor Angles */
    KineticFunctions.MinorAngles( k, A, B );
    
    /* Step 2: Calculate Quaternion Rotations */
    KineticFunctions.Quaternions( k );
    
    /* Step 3: Calculate Major Angles */
    KineticFunctions.MajorAngles( k );
    
    /* Step 4: Calculate r vector */
    KineticFunctions.R( k );
    
    /* Step 5B: Update Position Using Non-gravitational Data */
    KineticFunctions.Nongrav( k, n );
    
    //printf("Yaw: %4d | Nu: %4dº | Up: %4dº | Sig: %4dº | Chi: %4dº | Mu: %4dº | Gamma: %4dº |  | r_l: %.4f\n", (int)(k->e.z*RAD_TO_DEG), (int)(k->nu*RAD_TO_DEG), (int)(k->upsilon*RAD_TO_DEG), (int)(k->sigmaR*RAD_TO_DEG), (int)(k->chi*RAD_TO_DEG), (int)(k->mu*RAD_TO_DEG), (int)(k->gamma*RAD_TO_DEG), /* H_a: <%4d,%4d,%4d> (int)(a.x), (int)(a.y), (int)(a.z),*/ k->r_l);
    return;
}

static void KineticMinorAngles( kinetic_t * k, kpoint_t A, kpoint_t B )
{
    KPoint.copy(&A, &k->A);
    KPoint.copy(&B, &k->B);
    /* Get distance between beacons and origin */
    KPoint.init( &k->A, k->W/2, k->H/2, PIXEL_TO_UNIT, k->f_l );
    KPoint.init( &k->B, k->W/2, k->H/2, PIXEL_TO_UNIT, k->f_l );
    kpoint_t d_ = { ( k->B.x - k->A.x ), ( k->B.y - k->A.y ) };
    k->d__l = KPoint.dist( &d_ );
    
    /* Get beacon tilt on camera plane */
    k->omega = KPoint.angl( &d_ );
    
    /* Get angle between A and frame */
    k->sigmaA = KPoint.anga( &k->A );
    
    vec3_t Av, Bv;
    KPoint.toVec3( &k->A, &Av );
    KPoint.toVec3( &k->B, &Bv );
    /* Get angle between beacons */
    k->sigmaR = Vector.ang3( &Av, &Bv );
    
    //printf("Omega: %6.3fº | SigmaA: %6.3fº | SigmaR: %6.3fº\n", k->omega*RAD_TO_DEG, k->sigmaA*RAD_TO_DEG, k->sigmaR*RAD_TO_DEG);
}

static void KineticQuaternions( kinetic_t * k )
{
    /* Rotate beacon A around origin by roll(r.y) and calculate nu and upsilon as horizonatl and vertical angle offset */
    floating_t cos_ry = cos(-k->e.y), sin_ry = sin(-k->e.y);
    k->nu      = atan2( ( sin_ry * k->A.x + cos_ry * k->A.y ), k->f_l );
    k->upsilon = atan2( ( sin_ry * k->A.y - cos_ry * k->A.x ), k->f_l );
    
    /* Generate Camera-Beacon quaternion */
    ang3_t a = { k->e.x + k->nu, k->e.y + k->omega, k->e.z + k->upsilon };
    Quaternion.fromEuler( &a, &k->qd_ );
    Quaternion.combine( &k->qd_, &k->qc, &k->qa );
    
    ang3_t b = { k->e.x, k->e.y + k->omega, k->e.z };
    Quaternion.fromEuler( &b, &k->qc_ );
    
/* OP1: Potential parallelization */
//    Quaternion.fromEuler( &k->e, &k->qd );
//    Quaternion.fromEuler( &a, &k->qd_ );
//    Quaternion.fromEuler( &b, &k->qc_ );
//    Quaternion.combine( &k->qd_, &k->qc, &k->qa );
}

static void KineticMajorAngles( kinetic_t * k )
{
    KineticFunctions.Chi( k );
    KineticFunctions.Mu(  k );
    KineticFunctions.Gamma( k );
}

/* Calculation of chi - Secondary angle of inner triangle
 * a - focal length
 * b - length of vec d'
 */
static int KineticChi( kinetic_t * k )
{
    floating_t m, n;
    if( ( fabs( k->sigmaA ) != M_PI_2 ) && ( k->d__l > 0 ) )
    {
        m = k->f_l / cos(k->sigmaA);
        n = m / k->d__l;
        k->chi = asin( n * sin(k->sigmaR) );
        return 0;
    }
    else k->chi = M_PI_2;
    return 0;
}

/* Calculation of mu - Real-visualized d vector offset angle
 * a - quaternion x factor
 * b - quaternion z factor
 */
static int KineticMu( kinetic_t * k )
{
    floating_t m, n, a = k->qc_.x, b = k->qc_.z;
    m = ( a * a ) + ( b * b );
    n = 1.0 - ( 2 * m );
    //OP2A: k->mu = -RASIN( n );
    if( fabs(n) < 1 )
    {
        k->mu = asin( n );
        return 1;
    }
    else k->mu = SIGN( n ) * M_PI_2;
    return 0;
}

/* Calculation of gammama */
static void KineticGamma( kinetic_t * k )
{
    //OP2B: k->gamma = k->chi - k->mu;
    k->gamma = k->chi + k->mu;
}

/* Calculation of r vec length
 * a - sin sigmaR
 * b - d vec real length
 */
static int KineticR_l( kinetic_t * k )
{
    floating_t m;
    if( fabs(k->sigmaR) < 1 )
    {
        m = sin( k->gamma ) / sin( k->sigmaR );
        k->r_l = m * k->d_l;
        return 1;
    }
    else k->r_l = -1;
    return 0;
}

/* Calculation of r vec */
static void KineticR( kinetic_t * k )
{
    KineticFunctions.R_l(k);
    vec3_t r_u = { 0, k->r_l, 0 };
    Quaternion.rotVec( &r_u, &k->qa, &k->r );
}

/* Calculation and filtering of nongraviation data */
static void KineticNongrav( kinetic_t * k, vec3_t * n )
{
#ifdef __KALMAN__
    Quaternion.inverse(&k->qd, &k->qd);
    Quaternion.rotVec( n, &k->qd, &k->n );
    
    Kalman.Step( &k->filters.position[0], k->r.i, k->n.i );
    Kalman.Step( &k->filters.position[1], k->r.j, k->n.j );
    Kalman.Step( &k->filters.position[2], k->r.k, k->n.k );
    
    k->values.position[0] = k->filters.position[0].value;
    k->values.position[1] = k->filters.position[1].value;
    k->values.position[2] = k->filters.position[2].value;
#else
    k->values.position[0] = k->r.i;
    k->values.position[1] = k->r.j;
    k->values.position[2] = k->r.k;
#endif
}

kinetic_functions KineticFunctions =
{
    .DefaultInit = KineticDefaultInit,
    .Init = KineticInit,
    .UpdateRotation = KineticUpdateRotation,
    .UpdatePosition = KineticUpdatePosition,
    .MinorAngles = KineticMinorAngles,
    .Quaternions = KineticQuaternions,
    .MajorAngles = KineticMajorAngles,
    .Chi = KineticChi,
    .Mu = KineticMu,
    .Gamma = KineticGamma,
    .R_l = KineticR_l,
    .R = KineticR,
    .Nongrav = KineticNongrav
};

/***************************************************************************************************
 *  \brief  Initialize Filters for Kinetic Data
 **************************************************************************************************/
#ifdef __KALMAN__
void Filters_Init( kinetic_t * k )
{
    for( uint8_t i = 0; i < 3; i++ )
    {
        Kalman.Initialize(&k->filters.rotation[i], 0.0, MOTION_MAX_KALMAN_LIFESPAN, ROTATION_MIN, ROTATION_MAX, DEFAULT_MOTION_UNCERTAINTY );
    }
    
    for( uint8_t i = 0; i < 3; i++ )
    {
        Kalman.Initialize(&k->filters.position[0], 0.0, MOTION_MAX_KALMAN_LIFESPAN, POSITION_MIN, POSITION_MAX, DEFAULT_MOTION_UNCERTAINTY );
        k->filters.position[i].acceleration_mode = true;
    }
}
#endif

void Camera_Rotation_Init( kinetic_t * k )
{
    ang3_t c_a = { CAMERA_OFFSET_ANGLE_X, CAMERA_OFFSET_ANGLE_Y, CAMERA_OFFSET_ANGLE_Z };
    Quaternion.fromEuler( &c_a, &k->qc );
}

void Reference_Rotation_Init( kinetic_t * k )
{
    ang3_t r_a = { REFERENCE_OFFSET_ANGLE_X, REFERENCE_OFFSET_ANGLE_Y, REFERENCE_OFFSET_ANGLE_Z };
    Quaternion.fromEuler( &r_a, &k->qr );
}

