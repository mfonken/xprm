import processing.serial.*;

float scale = 500;

float ellipse_radius = 5.0;

Serial myPort;  // Create object from Serial class

float [] Mag = new float[3];          //mag readings


int lf = 10; // 10 is '\n' in ASCII
byte[] inBuffer = new byte[100];
String state = "rf";

PFont font;
final int VIEW_SIZE_X = 600, VIEW_SIZE_Y = 600;

PrintWriter output;

int numSamples = 10000;

void setup() 
{
  size(1000, 1000);
  try {
    printArray(Serial.list());
    if (Serial.list().length < 3) throw new Exception();
    myPort = new Serial(this, Serial.list()[3], 115200);
  }
  catch (Exception e) {
    println("Could not open serial port.");
    exit();
    while (true);
  }
  // The font must be located in the sketch's "data" directory to load successfully
  font = loadFont("CourierNew36.vlw"); 
  background(0);
  stroke(255);
  line(0, height/2, width, height/2);
  line(width/2, 0, width/2, height);
  noStroke();
  ellipseMode(CENTER);

  output = createWriter("mag_output.txt");
  output.println((int)(0.8*numSamples));
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
        case 'm':
          print("Received Raw Data");
          Mag[0] = float(inputStringArr[1])/1000;
          print(".");
          Mag[1] = float(inputStringArr[2])/1000;
          print(".");
          Mag[2] = float(inputStringArr[3])/1000;
          println(".");

          if ( Float.isNaN(Mag[0]) || Float.isNaN(Mag[1]) || Float.isNaN(Mag[2])) throw new Exception();
          myPort.clear();
          println(numSamples);
          numSamples--;
          if (numSamples == -1) 
          {
            exit();
          }
          output.println(Mag[0] + " " + Mag[1] + " " + Mag[2]); 
          return 'r';

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

  readSensors();

  //output.println(Mag[0] + ", " + Mag[1] + ", " + Mag[2]); 
  drawPoint();
}

void drawPoint()
{
  /* Draw X-Y Point */
  fill(255, 0, 0);
  ellipse(Mag[0]*scale+width/2, Mag[1]*scale+height/2, ellipse_radius, ellipse_radius);

  /* Draw X-Z Point */
  fill(0, 255, 0);
  ellipse(Mag[0]*scale+width/2, Mag[2]*scale+height/2, ellipse_radius, ellipse_radius);

  /* Draw Y-Z Point */
  fill(0, 0, 255);
  ellipse(Mag[1]*scale+width/2, Mag[2]*scale+height/2, ellipse_radius, ellipse_radius);
}

void keyPressed()
{
  switch(key)
  {
  case 'c':
    println("Screen cleared.");
    background(0);
    break;
  case ' ':
    output.flush(); // Writes the remaining data to the file
    output.close(); // Finishes the file
    exit();
    while(true);
  default:
  }
}