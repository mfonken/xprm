int o_x = 0;
int o_y = 0;
int o_z = 0;

vec3_t v = new vec3_t();
vec3_t r = new vec3_t();
vec3_t m = new vec3_t();
ang3_t rot = new ang3_t();

float rot_x = PI/3;
float rot_y = 0;//PI/2;
float rot_z = PI/4;

void setup()
{
  size(600, 600, P3D);
  background(10);
  v.i = 30;
  v.j = 100;
  v.k = 30;
}

void draw()
{
  background(10);
  pushMatrix();
  translate(width/3,height/3,0);
  rot_x = mouseY/float(height) * PI;
  rot_z = mouseX/float(width) * PI - PI/4;
  rotateX(rot_x);
  rotateZ(rot_z);
  //rotateY();
  //rotateY(-PI/4);
  stroke(245);
  noFill();
  box(25);  
  stroke(255,0,0);
  line(0,0,0,200,0,0);
  stroke(0,255,0);
  line(0,0,0,0,200,0);
  stroke(0,0,255);
  line(0,0,0,0,0,200);
  
  stroke(100, 100, 100);
  line(o_x,o_y,o_z, v.i + o_x, v.j + o_y, v.k + o_z);
  popMatrix();
  
  pushMatrix();
  translate(0,width*4/5,0);
  noFill();
  stroke(245);
  rect(0,0,width/5, height/5);
  translate(width/10,height/10,0);
  fill(230);
  box(25);
  //m.i = v.i/2-v.i;
  //m.j = v.j/2-v.j;
  //m.k = v.k/2-v.k;
  m = v;
  rot.a = rot_y;// + PI/4;
  rot.b = rot_x;// + PI/2;
  rot.c = -rot_z;
  m = zxyTransform(m, rot);
  vec3_t s = new vec3_t();
  
  s.i = 50;
  s.j = 0;
  s.k = 0;
  vec3_t t = new vec3_t();
  ang3_t u = new ang3_t();
  stroke(255,0,0);
  u.a = PI/2;
  u.b = 0;
  u.c = 0;
  t = zxyTransform(s, u);
  line(0, 0, 0, t.i, t.j, t.k);
  stroke(0,255,0);
  u.a = 0;
  u.b = PI/2;
  u.c = 0;
  t = zxyTransform(s, u);
  line(0, 0, 0, t.i, t.j, t.k);
  stroke(0,0,255);
  u.a = 0;
  u.b = 0;
  u.c = -PI/2;
  t = zxyTransform(s, u);
  line(0, 0, 0, t.i, t.j, t.k);
  stroke(150,150,150);
  line(m.i, m.j, m.k, -m.i, -m.j, -m.k);  
  popMatrix();
  
  
  pushMatrix();
  stroke(245);
  fill(245);
  text("Rx: " + rot_x, width*4/5, 12, 0);
  text("Ry: " + rot_y, width*4/5, 24, 0);
  text("Rz: " + rot_z, width*4/5, 36, 0);
  line(width/5,height*4/5,width,0);
  stroke(100, 100, 100);
  translate(width*2/3, height*2/3, 0);
  rotateX(PI/3);
  rotateZ(PI/4);
    
  rotateY(((float)millis())/10000 * PI);
  
  stroke(245);
  noFill();
  box(25);
  line(0, 0, 0, v.i, v.j, v.k);
  
  rot.a = rot_y;
  rot.b = 0;
  rot.c = 0;
  stroke(255, 255, 0);
  r = zxyTransform(v, rot);
  line(0, 0, 0, r.i, r.j, r.k);
  text("Y", r.i+10,r.j,r.k);
  
  
  rot.a = 0;
  rot.b = rot_x;
  rot.c = 0;
  stroke(255, 0, 255);
  r = zxyTransform(v, rot);
  line(1, 0, 0, r.i+1, r.j, r.k);  
  text("X", r.i+5,r.j,r.k);
  
  rot.a = 0;
  rot.b = 0;
  rot.c = -rot_z;
  stroke(0, 255, 255);
  r = zxyTransform(v, rot);
  line(2, 0, 0, r.i+2, r.j, r.k);
  text("Z", r.i-10,r.j,r.k);
  
  rot.a = -rot_y;
  rot.b = -rot_x;
  rot.c = -rot_z;
  stroke(255, 100, 0);
  r = zxyTransform(v, rot);
  line(3, 0, 0, r.i+3, r.j, r.k);  
  text("R", r.i-10,r.j,r.k);
  popMatrix();
}

void mousePressed()
{
  rotateY(PI/3 + mouseY/float(height) * PI);
}

void keyPressed()
{
  println("v(" + v.i + ", " + v.j + ", " + v.k + ") -> r(" + m.i + ", " + m.j + ", " + m.k + ")");
  //println("||v|| = " + lengthOfvec3_t(v) + " | ||r|| = " + lengthOfvec3_t(r));
}

vec3_t zxyTransform( vec3_t x, ang3_t rot)
{
    /* Extract angles */
    float a = rot.a;
    float b = rot.b;
    float c = rot.c;

    /* Transformation Matrix */
    float A[][] = new float[3][3];
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
    float y[] = new float[3];
    for( int i = 0; i < 3; i++ )
    {
        y[i] = ( A[i][0] * x.i ) + ( A[i][1] * x.j ) + ( A[i][2] * x.k );
    }
    vec3_t yvec = new vec3_t();
    yvec.i = y[0];
    yvec.j = y[1];
    yvec.k = y[2];
    return yvec;
}

float lengthOfvec3_t( vec3_t vec )
{
    float i_2 = vec.i * vec.i;
    float j_2 = vec.j * vec.j;
    float k_2 = vec.k * vec.k;
    return sqrt( i_2 + j_2 + k_2 );
}