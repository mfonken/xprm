import processing.serial.*;

float DECLINATION = -8.5;

float axis_scale = 5;

Serial myPort;  // Create object from Serial class

float [] Accel = new float[3];         //projection of normalized gravitation force vector on x/y/z axis, as measured by accelerometer
float [] Gyro = new float[3];          //Gyro readings
float [] Mag = new float[3];          //Gyro readings
float [] RwGyro = new float[3];        //Rw obtained from last estimated value and gyro movement
float [] Awz = new float[2];           //angles between projection of R on XZ/YZ plane and Z axis (deg)
float [] RwEst = new float[3];

float [] MAG_CAL = { 0, 0, 0 };

int lf = 10; // 10 is '\n' in ASCII
byte[] inBuffer = new byte[100];
String state = "rf";

PFont font;
final int VIEW_SIZE_X = 600, VIEW_SIZE_Y = 600;

int IND_AXES_X = 100;
int IND_AXES_Y = 550;

int graph_loc = 100;
int graph_height = 100;
int graph_x = 0;
float ACCEL_SCALE = graph_height;
float TIME_SCALE = 1000;
int [] prev = { 0, 0, 0 };
int [] pk   = { 0, 0, 0 };

kalman_t [] af = new kalman_t[3];

int skip = 3;

void graph()
{
  RwEst[0] *= RAD_TO_DEG;
  RwEst[1] *= RAD_TO_DEG;
  RwEst[2] *= RAD_TO_DEG;

  println("Roll is " + RwEst[0]);
  int y;
  double dt;

  stroke(color(255, 0, 0));
  y = (int)(RwEst[0]) + graph_loc + graph_height/2;
  line(graph_x-skip, prev[0], graph_x, y);
  prev[0] = y;

  stroke(color(255, 255, 0));

  double v = af[0].value;
  if ( ( RwEst[0] < -90 && v >  90 ) ||
    ( RwEst[0] >  90 && v < -90 ) )
  {
    af[0].value  = RwEst[0];
  } else
  {
    dt = millis() - af[0].timestamp;
    /* Calculate the true pitch using a kalman_t filter */
    Kalman_Update(af[0], RwEst[0], Gyro[0], dt/TIME_SCALE);
  }

  if ( af[0].value > HALF_PI )
  {
    /* Invert rate, so it fits the restricted accelerometer reading */
    Gyro[0] = -Gyro[0];
  }



  y = (int)(af[0].value) + graph_loc + graph_height/2;
  line(graph_x-skip, pk[0], graph_x, y);
  pk[0] = y;

  stroke(color(0, 255, 0));
  y = (int)(RwEst[1]) + graph_loc*2 + graph_height/2;
  line(graph_x-skip, prev[1], graph_x, y);
  prev[1] = y;

  stroke(color(0, 255, 255));
  dt = millis() - af[1].timestamp;
  println("∆t is " + dt);
  Kalman_Update(af[1], RwEst[1], Gyro[1], dt/TIME_SCALE);
  y = (int)(af[1].value) + graph_loc*2 + graph_height/2;
  line(graph_x-skip, pk[1], graph_x, y);
  pk[1] = y;

  stroke(color(0, 0, 255));
  y = (int)(RwEst[2]) + graph_loc*3 + graph_height/2;
  line(graph_x-skip, prev[2], graph_x, y);
  prev[2] = y;

  stroke(color(255, 0, 255));
  dt = millis() - af[2].timestamp;
  println("∆t is " + dt);
  Kalman_Update(af[2], RwEst[2], Gyro[2], dt/TIME_SCALE);
  y = (int)(af[2].value) + graph_loc*3 + graph_height/2;
  line(graph_x-skip, pk[2], graph_x, y);
  pk[2] = y;
}

void setup() 
{
  size(600, 600, P3D);
  try {
    printArray(Serial.list());
    if (Serial.list().length < 3) throw new Exception();
    myPort = new Serial(this, Serial.list()[3], 9600);
  }
  catch (Exception e) {
    println("Could not open serial port.");
    exit();
    while (true);
  }
  // The font must be located in the sketch's "data" directory to load successfully
  font = loadFont("CourierNew36.vlw"); 

  af[0] = new kalman_t();
  af[1] = new kalman_t();
  af[2] = new kalman_t();

  Kalman_Init(af[0], 0);
  Kalman_Init(af[1], 0);
  Kalman_Init(af[2], 0);

  background(#000000);
}

void readSensors() {
  try {
    if (myPort.available() > 0) {
      if (myPort.readBytesUntil(lf, inBuffer) > 0) {
        String inputString = new String(inBuffer);
        String [] inputStringArr = split(inputString, ',');
        print("Input string: " + inputStringArr.length + "|" + inputString );

        switch(inputStringArr[0].charAt(0))
        {
        case 'r':

          //println("Received Raw Data.");
          // convert raw readings to G
          Accel[0] = float(inputStringArr[1]);
          Accel[1] = float(inputStringArr[2]);
          Accel[2] = float(inputStringArr[3]);

          // convert raw readings to degrees/sec
          Gyro[0] = float(inputStringArr[4]);
          Gyro[1] = float(inputStringArr[5]);
          Gyro[2] = float(inputStringArr[6]);

          Mag[0] = float(inputStringArr[7]) - MAG_CAL[0];
          Mag[1] = float(inputStringArr[8]) - MAG_CAL[1];
          Mag[2] = float(inputStringArr[9]) - MAG_CAL[2];
          calculateAngles();

          if ( Float.isNaN(Accel[0]) || Float.isNaN(Accel[1]) || Float.isNaN(Accel[2])) throw new Exception();
          if ( Float.isNaN(Gyro[0]) || Float.isNaN(Gyro[1]) || Float.isNaN(Gyro[2])) throw new Exception();
          if ( Float.isNaN(Mag[0]) || Float.isNaN(Mag[1]) || Float.isNaN(Mag[2])) throw new Exception();
          myPort.clear();
          graph_x+=skip;
          graph();
          break;
        case 'f':
          println("Received Filtered Data.");
          RwEst[0] = float(inputStringArr[1]);
          RwEst[1] = float(inputStringArr[2]);
          RwEst[2] = float(inputStringArr[3]);
          myPort.clear();
          //return 'f';
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
}

void draw() {  
  readSensors();

  if (graph_x == width) 
  {
    graph_x = 0;
    println("Resetting graph x");
    background(#000000);
  }

  //drawText();
  //drawAxes();
  //drawCube();
}

void keyTyped() 
{
  switch(key)
  {
  case 'r':
    myPort.write('z');
    println("Resetting.");
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
}