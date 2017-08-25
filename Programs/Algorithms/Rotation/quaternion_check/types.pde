class quaternion_t
{
  float w;
  float x;
  float y;
  float z;
}

class vec3_t
{
    float    i;             /**< \$f\hat{i}\$f value */
    float    j;             /**< \$f\hat{j}\$f value */
    float    k;             /**< \$f\hat{k}\$f value */
}
    
/*! Simple 3d Vector */
class mat3x3_t
{
    float    ii;               /**< \$f\hat{ii}\$f value */
    float    ji;               /**< \$f\hat{ji}\$f value */
    float    ki;               /**< \$f\hat{ki}\$f value */
    float    ij;               /**< \$f\hat{ij}\$f value */
    float    jj;               /**< \$f\hat{jj}\$f value */
    float    kj;               /**< \$f\hat{kj}\$f value */
    float    ik;               /**< \$f\hat{ik}\$f value */
    float    jk;               /**< \$f\hat{jk}\$f value */
    float    kk;               /**< \$f\hat{kk}\$f value */
}