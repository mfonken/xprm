import processing.serial.*;

int rot_step = 5;
int axis_scale = 100;
int BASE_MIN = -60;
int BASE_MAX = 60;
int ARM_MIN = -60;
int ARM_MAX = 60;

float DECLINATION = -8.5;

Serial myPort;  // Create object from Serial class

String PORT = "COM19";

float [] Accel = new float[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float [] Gyro = new float[3];          //Gyro readings
float [] Mag = new float[3];          //Gyro readings
float [] RwGyro = new float[3];        //Rw obtained from last estimated value and gyro movement
float [] Awz = new float[2];           //angles between projection of R on XZ/YZ plane and Z axis (deg)
float [] RwEst = new float[3];

float [] MAG_CAL = { 0.470, 0.120, 0.125 };

float armRot = 0;
float baseRot = 0;

int lf = '\n';
byte[] inBuffer = new byte[100];
String state = "rf";

PFont font;
final int VIEW_SIZE_X = 600, VIEW_SIZE_Y = 600;

int IND_AXES_X = 100;
int IND_AXES_Y = 550;

void setup() 
{
  size(600, 600, P3D);
  try {
    String ports[] = Serial.list();
    printArray(ports);
    for (int i = 0; i < ports.length; i++)
      if (ports[i].equals(PORT)) myPort = new Serial(this, Serial.list()[i], 115200);
  }
  catch (Exception e) {
    println("Could not open serial port.");
    exit();
    while (true);
  }
  // The font must be located in the sketch's "data" directory to load successfully
  font = loadFont("CourierNew36.vlw"); 
  myPort.write('i');
  myPort.write('r');
}

char readSensors() {
  try {
    if (myPort.available() > 0) {
      if (myPort.readBytesUntil(lf, inBuffer) > 0) {
        String inputString = new String(inBuffer);
        String [] inputStringArr = split(inputString, ',');
        println("Input string: " + inputStringArr.length + "|" + inputString );
        switch(inputStringArr[0].charAt(0))
        {
        case 'r':
          println("Received Raw Data.");
          Accel[0] = float(inputStringArr[1]);
          Accel[1] = float(inputStringArr[2]);
          Accel[2] = float(inputStringArr[3]);

          Gyro[0] = float(inputStringArr[4]);
          Gyro[1] = float(inputStringArr[5]);
          Gyro[2] = float(inputStringArr[6]);

          Mag[0] = float(inputStringArr[7]) - MAG_CAL[0];
          Mag[1] = float(inputStringArr[8]) - MAG_CAL[1];
          Mag[2] = float(inputStringArr[9]) - MAG_CAL[2];

          myPort.clear();
          normalizeValues();
          calculateAngles();

          return 'r';
        case 'f':
          println("Received Filtered Data.");
          RwEst[0] = float(inputStringArr[1]);
          RwEst[1] = float(inputStringArr[2]);
          RwEst[2] = float(inputStringArr[3]);
          myPort.clear();
          return 'f';
        case 'n':
          println("Received Filtered Data.");
          RwEst[0] = float(inputStringArr[1]);
          RwEst[1] = float(inputStringArr[2]);
          RwEst[2] = float(inputStringArr[3]);
          myPort.clear();
          return 'f';
        case 'i':
          armRot = float(inputStringArr[1]);
          baseRot = float(inputStringArr[2]);
          println("Setting rotations: Arm-" + armRot + " and Base-" + baseRot);
        case 'w':
          println("Wrote:" + inputStringArr[1] + " " + inputStringArr[2]);
          break;
        default:
          break;
        }
      }
    }
  }
  catch (Exception ex) 
  {
    println("Error!");
  }
  myPort.clear();
  return 'n';
}

void draw() {  
  myPort.write(state);
  char ret = readSensors();

  background(#000000);
  fill(#ffffff);

  textFont(font, 20);
  //float temp_decoded = 35.0 + ((float) (temp + 13200)) / 280;
  //text("temp:\n" + temp_decoded + " C", 350, 250);
  text("Accel (g):\n" + Accel[0] + "\n" + Accel[1] + "\n" + Accel[2], 20, 50);
  text("Gyro (deg/s):\n" + Gyro[0] + "\n" + Gyro[1] + "\n" + Gyro[2], 220, 50);
  text("Mag (gauss):\n" + Mag[0] + "\n" + Mag[1] + "\n" + Mag[2], 420, 50);
  text("RwEst :\n" + RwEst[0] + "\n" + RwEst[1] + "\n" + RwEst[2], 20, 180);

  // display axes
  pushMatrix();
  translate(450, 250, 0);
  stroke(#ffffff);
  line(0, 0, 0, 1*axis_scale, 0, 0);
  line(0, 0, 0, 0, -1*axis_scale, 0);
  line(0, 0, 0, 0, 0, 1*axis_scale);
  stroke(#ffaa00);
  line(0, 0, 0, RwEst[0]*axis_scale, -RwEst[1]*axis_scale, RwEst[2]*axis_scale);
  stroke(#ff0000);
  line(0, 0, 0, -cos(RwEst[2])*axis_scale, sin(RwEst[2])*axis_scale, 0);
  stroke(#ffaaaa);
  noFill();
  ellipse(0, 0, 2*axis_scale, 2*axis_scale);
  text("N", -5, -axis_scale);
  text("S", -5, 12+axis_scale);
  text("E", axis_scale, 5);
  text("W", -12-axis_scale, 5);
  popMatrix();

  drawCube();
}

void keyTyped() 
{
  switch(key)
  {
  case 'a':
    if (rot_step > BASE_MIN)
      baseRot-=rot_step;
    break;
  case 'd':
    if (rot_step < BASE_MAX)
      baseRot+=rot_step;
    break;
  case 'w':
    if (rot_step < ARM_MAX)
      armRot+=rot_step;
    break;
  case 's':
    if (rot_step > ARM_MIN)
      armRot-=rot_step;
    break;
  case 'r':
    myPort.write('z');
    println("Resetting.");
    baseRot = 0;
    armRot = 0;
    break;
  case '1':
    state = "rd";
    break;
  case '2':
    state = "rf";
    break;
  case 'c':
    myPort.write('c');
    println("Centering.");
  default:
    break;
  }
  int arm = (int)armRot;
  int base = (int)baseRot;
  myPort.write("w " + arm + " " + base);
  println("Writing: " + arm + " " + base);
}

void buildBoxShape() {
  //box(60, 10, 40);
  noStroke();
  beginShape(QUADS);

  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-30, -5, 20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);

  //Z-
  fill(#0000ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, 5, -20);
  vertex(-30, 5, -20);

  //X-
  fill(#ff0000);
  vertex(-30, -5, -20);
  vertex(-30, -5, 20);
  vertex(-30, 5, 20);
  vertex(-30, 5, -20);

  //X+
  fill(#ffff00);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(30, 5, -20);

  //Y-
  fill(#ff00ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(-30, -5, 20);

  //Y+
  fill(#00ffff);
  vertex(-30, 5, -20);
  vertex(30, 5, -20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);

  endShape();
}

void buildPenShape() {

  int pen_w = 5;
  int pen_h = 5;
  int pen_l = 50;
  noStroke();
  beginShape(QUADS);

  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-pen_w, -pen_h, pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);

  //Z-
  fill(#0000ff);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, pen_h, -pen_l);
  vertex(-pen_w, pen_h, -pen_l);

  //X-
  fill(#ff0000);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(-pen_w, -pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, -pen_l);

  //X+
  fill(#ffff00);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(pen_w, pen_h, -pen_l);

  //Y-
  fill(#ff00ff);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(-pen_w, -pen_h, pen_l);

  //Y+
  fill(#00ffff);
  vertex(-pen_w, pen_h, -pen_l);
  vertex(pen_w, pen_h, -pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);

  endShape();
}

void drawCube() {  
  pushMatrix();
  translate(200, 400, 0);
  scale(4, 4, 4);

  rotateY( RwEst[2] + PI/2);
  rotateX(-RwEst[0]);
  rotateZ(-RwEst[1]);


  buildPenShape();
  popMatrix();

  pushMatrix();
  translate(IND_AXES_X, IND_AXES_Y, 0);
  rotateX(RwEst[0]);
  buildBoxShape();
  popMatrix();

  pushMatrix();
  translate(IND_AXES_X+100, IND_AXES_Y, 0);
  rotateZ(RwEst[1]);
  buildBoxShape();
  popMatrix();

  pushMatrix();
  translate(IND_AXES_X+200, IND_AXES_Y, 0);
  rotateY(RwEst[2]);
  buildBoxShape();
  popMatrix();
}

/******************************************************************************
 * Rotation Calculation
 *****************************************************************************/
/* See - http://www.nxp.com/files/sensors/doc/app_note/AN3461.pdf and
 - http://www.nxp.com/assets/documents/data/en/application-notes/AN4248.pdf
 */

void normalizeValues( )
{
  normalizeAccel();
  normalizeMag();
}

void normalizeAccel()
{
  float n = sqrt( Accel[0] * Accel[0] + Accel[1] * Accel[1] + Accel[2] * Accel[2] );
  Accel[0] /= n;
  Accel[1] /= n;
  Accel[2] /= n;
}

void normalizeMag()
{
  float n = sqrt( Mag[0] * Mag[0] + Mag[1] * Mag[1] + Mag[2] * Mag[2] );
  Mag[0] /= n;
  Mag[1] /= n;
  Mag[2] /= n;
}

void calculateAngles( )
{
  calculateRoll();
  calculatePitch();
  calculateYaw();
}
/**************************************************************************//**
 * \brief Calculate roll angle (phi) from accelerometer data
 * \param[out] Return roll
 *****************************************************************************/
void calculateRoll( )
{
  /* AN4248: Eq. 13 */
  RwEst[0] = atan2( Accel[0], Accel[2] );

  /* AN3461: Eq. 37 */
  //    double den = sqrt( ( ( this.imu.accel[1] * this.imu.accel[1] ) + ( this.imu.accel[2] * this.imu.accel[2] ) ) );
  //    this.imu.roll = atan2( -this.imu.accel[0], den );
}

/**************************************************************************//**
 * \brief Calculate pitch angle (theta) from accelerometer data
 * \param[out] Return pitch
 *****************************************************************************/
void calculatePitch( )
{
  /* AN4248: Eq. 14 */
  float den = ( ( Accel[0] * sin( RwEst[0] ) )  + ( Accel[2] * cos( RwEst[0] ) ) );
  RwEst[1] = atan2( -Accel[1], den );

  /* AN3461: Eq. 38 */
  //int s = Accel[2] > 0 ? 1:-1;
  //float MU = 0.01;
  //float den = s * sqrt( ( ( Accel[2] * Accel[2] ) + ( MU * ( Accel[0] * Accel[0] ) ) ) );
  //RwEst[1] = atan2( Accel[1], den );
}

/**************************************************************************//**
 * \brief Calculate yaw angle (psi) from magnetometer data, pitch, and roll
 * \param[out] Return yaw
 *****************************************************************************/
void calculateYaw( )
{
  /* AN4248: Eq. 22 */
  float sin_phi   = sin( RwEst[0] );
  float sin_theta = sin( RwEst[1] );
  float cos_phi   = cos( RwEst[0] );
  float cos_theta = cos( RwEst[1] );
  float num = ( -Mag[2] * sin_phi ) - ( Mag[1] * cos_phi );
  float den = ( -Mag[0] * cos_theta ) + ( Mag[1] * ( sin_theta * sin_phi ) ) + ( -Mag[2] * ( sin_theta * cos_phi ) );
  RwEst[2] = atan2( num, den );
}