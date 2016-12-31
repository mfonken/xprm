import processing.serial.*;

Serial myPort, com;
PrintWriter output;

int x_l = 32;
int y_l = 27;
int k = 0;

char bmp[] = new char[4000];

PrintWriter file;

char frame[][][] = new char[32][32][3];

int frame_counter = 0;
int num_pixels = x_l * 2 * y_l;

int NUM_FRAMES = 30;

void setup() {
  size(32, 27);
  background(245);
  println(width + "x" + height);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[3], 921600);
  com = new Serial( this, Serial.list()[4], 115200);

  println("Waiting for data");
  noStroke();

  file = createWriter("frame.csv");
}

void draw()
{
  while (myPort.available() > 0)
  {
    bmp[k] = myPort.readChar();
    //println("Current k is " + k + " and bmp[k] is " + bmp[k]);
    if ( k > 0 && bmp[k-1] == 0xab && bmp[k] == 0x34)
    {
      myPort.clear();
      com.write('y');
      readImage();
    }
    k++;
    if (k >= 4000) k = 0;
  }
  
}

void readImage()
{
  for (int y = 0; y < y_l; y++)
  {
    byte line[] = new byte[4000];
    int ln = 0xef12;
    myPort.readBytesUntil(ln, line);
    myPort.clear();
    com.write('x');
    for (int x = 0; x < x_l; x++)
    {
      int p = x * 2;
      char a = (char)line[p];
      char b = (char)line[p+1];
      int br = ( a + b) / 2;
      set(x, y, color(br));
      //println("<(" + x + ", " + y + "> " + (int)a + "|" + (int)b + "=" + br);
    }
  }
  frame_counter++;
  println(">" + frame_counter);
  if( frame_counter >= NUM_FRAMES ) 
  {
    println("Frames are done.");
    exit();
    while(true);
  }
  myPort.clear();
}






void keyPressed() {
  switch(key)
  {
  case 'w':
    x_l = x_l + 1;
    break;
  case 's':
    x_l = x_l - 1;
    break;
  case 'a':
    y_l = y_l - 1;
    break;
  case 'd':
    y_l = y_l + 1;
    break;
  case 'f':
    saveFrame();
    break;
  default:
    while (true);
  }
  background(245);
  println("New x is " + x_l + " and y is " + y_l);
  num_pixels = x_l * 2 * y_l;
}


void saveFrame()
{
  for (int y = 0; y < height; y++)
  {
    for (int x = 0; x < width; x++)
    {
      file.print("{" + (int)frame[y][x][0] + "|" + (int)frame[y][x][1] + "|" +  + (int)frame[y][x][2] + "},"); // Write the coordinate to the file
    }
    file.println();
  }
  file.flush(); // Writes the remaining data to the file
  file.close(); // Finishes the file
  println("Frame has been saved.");
}