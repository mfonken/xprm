void Multiply_Vec_3x1( mat3x3_t a, vec3_t b, vec3_t c )
{
    c.i = a.ii * b.i + a.ij * b.j + a.ik * b.k;
    c.j = a.ji * b.i + a.jj * b.j + a.jk * b.k;
    c.k = a.ki * b.i + a.kj * b.j + a.kk * b.k;
}

void Euler_To_Quaternion( float a[], quaternion_t quat )
{
    float half_roll, half_pitch, half_yaw;
    half_roll   = a[0] / 2;
    half_pitch  = a[1] / 2;
    half_yaw    = a[2] / 2;
    float cr, cp, cy, sr, sp, sy, cpcy, spsy, spcy, cpsy;
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
    quat.w = cr * cpcy + sr * spsy;
    quat.x = sr * cpcy - cr * spsy;
    quat.y = cr * spcy + sr * cpsy;
    quat.z = cr * cpsy - sr * spcy;
}

void Quaternion_To_Matrix(quaternion_t quat, mat3x3_t m)
{
    float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
    
    x2 = quat.x + quat.x; y2 = quat.y + quat.y;
    z2 = quat.z + quat.z;
    xx = quat.x * x2; xy = quat.x * y2; xz = quat.x * z2;
    yy = quat.y * y2; yz = quat.y * z2; zz = quat.z * z2;
    wx = quat.w * x2; wy = quat.w * y2; wz = quat.w * z2;
    
    m.ii = 1.0 - (yy + zz);
    m.ji = xy - wz;
    m.ki = xz + wy;
    
    m.ij = xy + wz;
    m.jj = 1.0 - (xx + zz);
    m.kj = yz - wx;
    
    m.ik = xz - wy;
    m.jk = yz + wx;
    m.kk = 1.0 - (xx + yy);
}

void Rotate_Vector_By_Quaternion(vec3_t v, quaternion_t q, vec3_t r)
{
    vec3_t u = new vec3_t();
    u.i = q.x;
    u.j = q.y;
    u.k = q.z;
    float s = q.w;
    vec3_t a, b, c, t;
    
    a = new vec3_t();
    b = new vec3_t();
    c = new vec3_t();
    t = new vec3_t();
    mul3( 2, u, a );
    cross3( a, v, t );
    cross3( u, t, b );
    mul3( s, t, c );
    add33( v, b, c, r );
}

void Quaternion_Combine(quaternion_t a, quaternion_t b, quaternion_t c, quaternion_t d)
{
    float A, B, C, D, E, F, G, H;
    A = ( a.w + a.x ) * ( b.w + b.x );
    B = ( a.z - a.y ) * ( b.y - b.z );
    C = ( a.w - a.x ) * ( b.y + b.z );
    D = ( a.y + a.z ) * ( b.w - b.x );
    E = ( a.x + a.z ) * ( b.x + b.y );
    F = ( a.x - a.z ) * ( b.x - b.y );
    G = ( a.w + a.y ) * ( b.w - b.z );
    H = ( a.w - a.y ) * ( b.w + b.z );
    d.w = B + ( -E - F + G + H ) / 2;
    d.x = A - (  E + F + G + H ) / 2;
    d.y = C + (  E - F + G - H ) / 2;
    d.z = D + (  E - F - G + H ) / 2;
    
    A = ( d.w + d.x ) * ( c.w + c.x );
    B = ( d.z - d.y ) * ( c.y - c.z );
    C = ( d.w - d.x ) * ( c.y + c.z );
    D = ( d.y + d.z ) * ( c.w - c.x );
    E = ( d.x + d.z ) * ( c.x + c.y );
    F = ( d.x - d.z ) * ( c.x - c.y );
    G = ( d.w + d.y ) * ( c.w - c.z );
    H = ( d.w - d.y ) * ( c.w + c.z );
    d.w = B + ( -E - F + G + H ) / 2;
    d.x = A - (  E + F + G + H ) / 2;
    d.y = C + (  E - F + G - H ) / 2;
    d.z = D + (  E - F - G + H ) / 2;
}

void cross3( vec3_t u, vec3_t v, vec3_t r )
{
    r.i = u.j*v.k - u.k*v.j;
    r.j = u.k*v.i - u.i*v.k;
    r.k = u.i*v.j - u.j*v.i;
}

/* Multiply a vector by a scalar */
void mul3( float m, vec3_t v, vec3_t r )
{
    r.i = m * v.i;
    r.j = m * v.j;
    r.k = m * v.k;
}

/* Add three vectors */
void add33( vec3_t u, vec3_t v, vec3_t w, vec3_t r)
{
    r.i = u.i + v.i + w.i;
    r.j = u.j + v.j + w.j;
    r.k = u.k + v.k + w.k;
}

void Multiply_Matrix_33x33( mat3x3_t a, mat3x3_t b, mat3x3_t c )
{
    c.ii = a.ii * b.ii + a.ij * b.ji + a.ik * b.ki;
    c.ij = a.ii * b.ij + a.ij * b.jj + a.ik * b.kj;
    c.ik = a.ii * b.ik + a.ij * b.jk + a.ik * b.kk;
    c.ji = a.ji * b.ii + a.jj * b.ji + a.jk * b.ki;
    c.jj = a.ji * b.ij + a.jj * b.jj + a.jk * b.kj;
    c.jk = a.ji * b.ik + a.jj * b.jk + a.jk * b.kk;
    c.ki = a.ki * b.ii + a.kj * b.ji + a.kk * b.ki;
    c.kj = a.ki * b.ij + a.kj * b.jj + a.kk * b.kj;
    c.kk = a.ki * b.ik + a.kj * b.jk + a.kk * b.kk;
}

void Multiply_Matrix_33x33x33( mat3x3_t a, mat3x3_t b, mat3x3_t c, mat3x3_t d )
{
    mat3x3_t t = new mat3x3_t();
    Multiply_Matrix_33x33(a,b,t);
    Multiply_Matrix_33x33(t,c,d);
}

void Euler_To_Rotation_Matrix( float a[], mat3x3_t m )
{
    float cx = cos(a[0]);
    float cy = cos(a[1]);
    float cz = cos(a[2]);
    float sx = sin(a[0]);
    float sy = sin(a[1]);
    float sz = sin(a[2]);
    
    mat3x3_t x, y, z;
    x = new mat3x3_t();
    y = new mat3x3_t();
    z = new mat3x3_t();
    
    x.ii =  1;
    x.ij =  0;
    x.ik =  0;
    x.ji =  0;
    x.jj =  cx;
    x.jk = -sx;
    x.ki =  0;
    x.kj = sx;
    x.kk = cx;
    
    y.ii =  cy;
    y.ij =  0;
    y.ik =  sy;
    y.ji =  0;
    y.jj =  1;
    y.jk =  0;
    y.ki = -sy;
    y.kj =  0;
    y.kk =  cy;
    
    z.ii =  cz;
    z.ij = -sz;
    z.ik =  0;
    z.ji =  sz;
    z.jj =  cz;
    z.jk =  0;
    z.ki =  0;
    z.kj =  0;
    z.kk =  1;
    
    Multiply_Matrix_33x33x33(z,y,x,m);
}

void Rotation_Matrix_To_Euler( mat3x3_t m, float a[] )
{
    a[0] = atan2(  m.kj, m.kk );
    a[1] = atan2( -m.ki, sqrt( ( m.kj * m.kj ) + ( m.kk * m.kk ) ) );
    a[2] = atan2(  m.ji, m.ii );
}