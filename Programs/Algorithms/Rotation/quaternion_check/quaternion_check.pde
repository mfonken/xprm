float rot_x, rot_y, rot_z;
int G_SCALE = 200;
int Q_SCALE = 170;
int E_SCALE = 150;

float a[] = {-PI/12,PI/2,PI/3};
float b[] = {-PI/3,PI/4,PI*5/6};
float c[] = {PI/4,-PI/4,PI/4};
float d[] = new float[3];
float r[] = new float[3];

quaternion_t H_a = new quaternion_t();
quaternion_t H_b = new quaternion_t();
quaternion_t H_c = new quaternion_t();
quaternion_t H_d = new quaternion_t();
quaternion_t H_r = new quaternion_t();

mat3x3_t m_a = new mat3x3_t();
mat3x3_t m_b = new mat3x3_t();
mat3x3_t m_c = new mat3x3_t();
mat3x3_t m_d = new mat3x3_t();
mat3x3_t m_r = new mat3x3_t();

void setup() 
{
  size(500, 500, P3D);
  frameRate(30);
  background(15);
}
void draw() 
{
  background(15);
  drawOriginThings();
  drawRotationThings();
}



void drawRotationThings()
{
  drawQuaterionTest();
  drawEulerTest();
}

void drawQuaterionTest()
{
  Euler_To_Quaternion(a, H_a);
  Euler_To_Quaternion(b, H_b);
  Euler_To_Quaternion(c, H_c);

  Quaternion_Combine(H_a, H_b, H_c, H_d);
  drawQuaternion(H_d, Q_SCALE, color(255, 100, 0));
}

void drawEulerTest()
{
  Euler_To_Rotation_Matrix(a, m_a);
  Euler_To_Rotation_Matrix(b, m_b);
  Euler_To_Rotation_Matrix(c, m_c);
  Multiply_Matrix_33x33(m_a, m_b, m_d);
  Multiply_Matrix_33x33(m_d, m_c, m_r);

  Rotation_Matrix_To_Euler(m_r, r);

  Euler_To_Quaternion(r, H_r);
  drawQuaternion(H_r, E_SCALE, color(100, 0, 255));
}

void drawQuaternion(quaternion_t q, int len, color c)
{
  vec3_t u = new vec3_t();
  vec3_t v = new vec3_t();
  u.i = 1; 
  u.j = 0; 
  u.k = 0;
  Rotate_Vector_By_Quaternion(u, q, v);
  drawVector(v, len, c);
}

void drawVector(vec3_t u, int len, color c)
{
  vec3_t v = new vec3_t();
  mul3(len, u, v);
  pushMatrix();
  strokeWeight(1);
  translate(width/2, height*2/3, 0);
  rot_x = mouseY/float(height) * PI;
  rot_z = mouseX/float(width) * PI - PI/4;
  rotateX(rot_x);
  rotateZ(rot_z);
  stroke(245);
  noFill();
  box(25);  
  stroke(c);
  line(0, 0, 0, v.i, v.j, -v.k);
  popMatrix();
}

void drawOriginThings()
{
  pushMatrix();
  strokeWeight(1);
  translate(width/2, height*2/3, 0);
  rot_x = mouseY/float(height) * PI;
  rot_z = mouseX/float(width) * PI - PI/4;
  rotateX(rot_x);
  rotateZ(rot_z);
  stroke(245);
  noFill();
  box(25);  
  stroke(255, 0, 0);
  line(0, 0, 0, G_SCALE, 0, 0);
  stroke(0, 255, 0);
  line(0, 0, 0, 0, G_SCALE, 0);
  stroke(0, 0, 255);
  line(0, 0, 0, 0, 0, G_SCALE);
  popMatrix();
}