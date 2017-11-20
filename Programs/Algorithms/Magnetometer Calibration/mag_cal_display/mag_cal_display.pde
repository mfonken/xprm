import processing.serial.*;

float scale = 500;
float ellipse_radius = 5.0;

Serial myPort;

float [] Mag = new float[3];          //mag readings

int lf = '\n';
byte[] inBuffer = new byte[100];

PFont font;
PrintWriter output;

int numSamples = 5000;

void setup() 
{
  size(1000, 1000);
  try {
    printArray(Serial.list());
    myPort = new Serial(this, Serial.list()[0], 115200);
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
  output.println((int)(0.9*numSamples));
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
          print("Received Raw Data-");
          Mag[0] = float(inputStringArr[1]);
          Mag[1] = float(inputStringArr[2]);
          Mag[2] = float(inputStringArr[3]);
          
          if ( Float.isNaN(Mag[0]) || Float.isNaN(Mag[1]) || Float.isNaN(Mag[2])) throw new Exception();
          myPort.clear();
          println(numSamples);
          numSamples--;
          if (numSamples == -1) exit();
          output.println(Mag[0] + " " + Mag[1] + " " + Mag[2]); 
          println("M: " + Mag[0] + " " + Mag[1] + " " + Mag[2]); 
          drawPoint();
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
  return 'n';
}

void draw() 
{
  readSensors();
}

void drawPoint()
{
  //Mag[0] += 4.077;
  //Mag[1] -= 41.577;
  //Mag[2] += 1.039;
  float m0 = Mag[0]*scale+width/2;
  float m1 = Mag[1]*scale+width/2;
  float m2 = Mag[2]*scale+width/2;
  println("F: " + m0 + " " + m1 + " " + m2); 
  
  /* Draw X-Y Point */
  fill(255, 0, 0);
  ellipse(m0, m1, ellipse_radius, ellipse_radius);

  /* Draw X-Z Point */
  fill(0, 255, 0);
  ellipse(m0, m2, ellipse_radius, ellipse_radius);

  /* Draw Y-Z Point */
  fill(0, 0, 255);
  ellipse(m1, m2, ellipse_radius, ellipse_radius);
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