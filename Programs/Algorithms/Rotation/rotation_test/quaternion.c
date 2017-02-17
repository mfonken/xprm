#include <math.h>
#include <stdio.h>
#include <sys/time.h>

#define PI      M_PI//3.141596
#define RAD_TO_DEG  180 / PI


#define PHI_P   -PI/2
#define THT_P   -PI/6
#define PSI_P   PI/3

#define PHI_C   0
#define THT_C   0
#define PSI_C   0

#define THT_B   0//PI/4
#define PSI_B   PI/3

#define D_FIXED 1

#define DEBUG

typedef struct
{
    double x;
    double y;
    double z;
    double w;
} quaternion_t;

void EulerToQuat( quaternion_t * quat, double roll, double pitch, double yaw )
{
    double half_roll, half_pitch, half_yaw;
    half_roll   = roll  / 2;
    half_pitch  = pitch / 2;
    half_yaw    = yaw   / 2;
    double cr, cp, cy, sr, sp, sy, cpcy, spsy;
    // calculate trig identities
    cr = cos( half_roll  );
    cp = cos( half_pitch );
    cy = cos( half_yaw   );
    sr = sin( half_roll  );
    sp = sin( half_pitch );
    sy = sin( half_yaw   );
    cpcy = cp * cy;
    spsy = sp * sy;
    quat->w = cr * cpcy + sr * spsy;
    quat->x = sr * cpcy - cr * spsy;
    quat->y = cr * sp * cy + sr * cp * sy;
    quat->z = cr * cp * sy - sr * sp * cy;
}

void Quaternion_To_Matrix(quaternion_t * quat, double m[3][3])
{
    double wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    
    x2 = quat->x + quat->x; y2 = quat->y + quat->y;
    z2 = quat->z + quat->z;
    xx = quat->x * x2; xy = quat->x * y2; xz = quat->x * z2;
    yy = quat->y * y2; yz = quat->y * z2; zz = quat->z * z2;
    wx = quat->w * x2; wy = quat->w * y2; wz = quat->w * z2;
    
    m[0][0] = 1.0 - (yy + zz);
    m[1][0] = xy - wz;
    m[2][0] = xz + wy;
    
    m[0][1] = xy + wz;
    m[1][1] = 1.0 - (xx + zz);
    m[2][1] = yz - wx;
    
    m[0][2] = xz - wy;
    m[1][2] = yz + wx;
    m[2][2] = 1.0 - (xx + yy);
}

void Quaternion_Combine(quaternion_t * a, quaternion_t * b, quaternion_t * c, quaternion_t * d)
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
    d->w = B + ( -E - F + G + H ) / 2;
    d->x = A - (  E + F + G + H ) / 2;
    d->y = C + (  E - F + G - H ) / 2;
    d->z = D + (  E - F - G + H ) / 2;
    
    A = ( d->w + d->x ) * ( c->w + c->x );
    B = ( d->z - d->y ) * ( c->y - c->z );
    C = ( d->w - d->x ) * ( c->y + c->z );
    D = ( d->y + d->z ) * ( c->w - c->x );
    E = ( d->x + d->z ) * ( c->x + c->y );
    F = ( d->x - d->z ) * ( c->x - c->y );
    G = ( d->w + d->y ) * ( c->w - c->z );
    H = ( d->w - d->y ) * ( c->w + c->z );
    d->w = B + ( -E - F + G + H ) / 2;
    d->x = A - (  E + F + G + H ) / 2;
    d->y = C + (  E - F + G - H ) / 2;
    d->z = D + (  E - F - G + H ) / 2;
}

double dot3(double u[3], double v[3])
{
    return u[0] * v[0] + u[1] * v[1] + u[2] * v[2];
}

void cross3( double u[3], double v[3], double r[3] )
{
    r[0] = u[1]*v[2] - u[2]*v[1];
    r[1] = u[2]*v[0] - u[0]*v[2];
    r[2] = u[0]*v[1] - u[1]*v[0];
}

void mul3( int m, double v[3], double r[3] )
{
    r[0] = m * v[0];
    r[1] = m * v[1];
    r[2] = m * v[2];
}

void add33( double u[3], double v[3], double w[3], double r[3])
{
    r[0] = u[0] + v[0] + w[0];
    r[1] = u[1] + v[1] + w[1];
    r[2] = u[2] + v[2] + w[2];
}

double len3( double v[3])
{
    return sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
}

void multiplyVec3x1( double a[3][3], double b[3], double c[3] )
{
    double r[3] = {0,0,0};
    for( int i = 0; i < 3; i++ )
    {
        r[i] = a[i][0] * b[0] + a[i][1] * b[1] + a[i][2] * b[2];
    }
    for( int i = 0; i < 3; i++ )
    {
        c[i] = r[i];
    }
}

void Vector_Rotate_By_Quaternion(double v[3], quaternion_t * q, double r[3])
{
    double m[3][3];
    Quaternion_To_Matrix( q, m );
    multiplyVec3x1( m, v, r );
//    // Extract the vector part of the quaternion
//    double u[3];
//    u[0] = q->x;
//    u[1] = q->y;
//    u[2] = q->z;
//    
//    // Extract the scalar part of the quaternion
//    double s = q->w;
//    
//    double a[3], b[3], c[3], uv[3];
//    cross3( u, v, uv );
//    mul3( 2, uv, a );
//    mul3( s, u, b );
//    add33( v, a, b, r );
#ifdef DEBUG
    printf("q = ( %f, %f, %f, %f )\n", q->w, q->x, q->y, q->z );
    printf( "Lengths: %f | %f\n", len3(v), len3(r));
#endif
}

void printQuaternion( quaternion_t * q, char c )
{
#ifdef DEBUG
    printf("%c - \n\t", c);
    double r[3][3];
    Quaternion_To_Matrix( q, r );
    for( int i = 0; i < 3; i++ )
    {
        printf("( %.3f, %.3f, %.3f )\n\t", r[i][0], r[i][1], r[i][2] );
    }
    printf("\n");
#endif
}


void printVector( double v[3] )
{
    printf("v - \n\t");
    printf("( %.3f, %.3f, %.3f )\n\t", v[0], v[1], v[2]);
    printf("\n");
}

int main(int argc, char * args[])
{
    clock_t t1, t2;
    double diff, ret;
    t1 = clock();
    quaternion_t qp, qc, qb, qa;
    EulerToQuat( &qp, PHI_P, THT_P, PSI_P );
    EulerToQuat( &qc, PHI_C, THT_C, PSI_C );
    EulerToQuat( &qb,     0, 0,0);//THT_B, PSI_B );
    Quaternion_Combine( &qp, &qc, &qb, &qa );
    double alpha = acos( cos( THT_B ) * cos( PSI_B ) );
    double r[3][3];
    Quaternion_To_Matrix( &qa, r );
    double mu = acos( 1 - 2 * ( qa.y * qa.y + qa.z * qa.z ) );//( qa.w * qa.w ) - ( qa.x * qa.x ) - ( qa.y * qa.y ) + ( qa.z * qa.z ) );
    
    double sigma = cos( alpha - mu ) / sin( alpha ) * D_FIXED;
    
    double A,B,C,a,b,c;
    C = alpha * RAD_TO_DEG;
    B = mu * RAD_TO_DEG;
    A = 180 - B - C;
    c = D_FIXED;
    a = c / sin( C ) * sin( A );
    b = c / sin( C ) * sin( B );
    
    printf("a - %.3f\n", alpha * RAD_TO_DEG);
    printf("m - %.3f\n", mu * RAD_TO_DEG);
    printf("s - %.3f\n", sigma * RAD_TO_DEG);
    
    printf("A - %.3f\n", A);
    printf("B - %.3f\n", B);
    printf("C - %.3f\n", C);
    printf("a - %.3f\n", a);
    printf("b - %.3f\n", b);
    printf("c - %.3f\n", c);
    

    double v[3] = {0, sigma,  0};
    double o[3];
    Vector_Rotate_By_Quaternion( v, &qa, o );
    t2 = clock();
    
    printQuaternion( &qp,  'p' );
    printQuaternion( &qc,  'c' );
    printQuaternion( &qb,  'b' );
    printQuaternion( &qa,  'a' );
 
    printf("alpha - %.3fº\n", alpha * RAD_TO_DEG);
    printf("   mu - %.3fº\n", mu * RAD_TO_DEG);
    printf("sigma - %.3f\n", sigma);
    printVector( o );
    
    diff = ((double)(t2 - t1) / 1000000.0F ) * 1000;
    printf("Matrix Rotation - %.3fms\n", diff);
    
    return 0;
}
