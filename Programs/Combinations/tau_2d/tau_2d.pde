import processing.serial.*;

boolean live = true;

int rot_step = 5;
int axis_scale = 100;

float DECLINATION = -8.5;

Serial myPort;  // Create object from Serial class
String PORT = "COM19";
int xPos = 0;

float [] Accel = new float[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float [] Gyro = new float[3];          //Gyro readings
float [] Mag = new float[3];          //Gyro readings
float [] RwGyro = new float[3];        //Rw obtained from last estimated value and gyro movement
float [] Awz = new float[2];           //angles between projection of R on XZ/YZ plane and Z axis (deg)
float [] RwEst = new float[3];
float [] PwEst = new float[3];
float [] NwEst = new float[3];

//float [] MAG_CAL = {-0.082, 0.384, 0.087}; // LSM9DS1
float [] MAG_CAL = {-2.396, 38.040, 1.093}; // MPU9250
int tx, ty;

int lf = '\n';
byte[] inBuffer = new byte[100];
String state = "rf";

PFont font;

int IND_AXES_X = 100;
int IND_AXES_Y = 550;
int IND_AXES_W = 80;
int IND_AXES_H = 20;

enum OUTPUTS {
  ACCEL,
  GYRO,
  MAG,
  ANG,
  POS
}

kalman_t pitch, roll, yaw;

void setup() 
{
  size(600, 1000);
  try {
    String ports[] = Serial.list();
    printArray(ports);
    for(int i = 0; i < ports.length; i++)
      if(ports[i].equals(PORT)) myPort = new Serial(this, Serial.list()[i], 115200);
  }
  catch (Exception e) {
    println("Could not open serial port.");
    exit();
    while (true);
  }
  // The font must be located in the sketch's "data" directory to load successfully
  font = loadFont("ArialMT-32.vlw");
  
  background(0);
  
  println("Sending start...");
  myPort.write('S');
  println("Waiting for handshake...");
  while(myPort.available() <= 0) myPort.write('S');
  println("Handshake successful...");
  
  pitch = new kalman_t();
  roll = new kalman_t();
  yaw = new kalman_t();
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
          // convert raw readings to G
          Accel[0] = float(inputStringArr[1]);
          Accel[1] = float(inputStringArr[2]);
          Accel[2] = float(inputStringArr[3]);

          // convert raw readings to degrees/sec
          Gyro[0] = float(inputStringArr[4]);
          Gyro[1] = float(inputStringArr[5]);
          Gyro[2] = float(inputStringArr[6]);

          Mag[1] =  float(inputStringArr[7]) - MAG_CAL[0];
          Mag[0] =  float(inputStringArr[8]) - MAG_CAL[1];
          Mag[2] = -float(inputStringArr[9]) - MAG_CAL[2];

          normalizeValues();
          calculateAngles();
          filterAngles();
          drawGraphs(OUTPUTS.MAG);
          return 'r';
        case 'f':
          println("Received Filtered Data.");
          RwEst[1] = float(inputStringArr[1]);
          RwEst[0] = float(inputStringArr[2]);
          RwEst[2] = float(inputStringArr[3]);
          
          PwEst[0] = float(inputStringArr[4]);
          PwEst[1] = float(inputStringArr[5]);
          PwEst[2] = float(inputStringArr[6]);
          
          //drawPosition();
          drawGraphs(OUTPUTS.POS);
          myPort.clear();
          return 'f';
        case 'n':
          println("Received Non-grav Acceleration Data.");
          NwEst[1] = float(inputStringArr[1]);
          NwEst[0] = float(inputStringArr[2]);
          NwEst[2] = float(inputStringArr[3]);
          drawPosition();
          break;
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

void draw() 
{
  readSensors();
  drawCompass();
  drawTilts();
}

void keyPressed()
{
  switch(key)
  {
    case ' ':
      live = !live;
      if(live) println("Running...");
      else println("Pausing...");

      break;
    default:
      break;
  }
}