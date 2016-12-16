import processing.serial.*;

Serial myPort;
PrintWriter output;

int pixel = 0;
int x_l = 32;
int y_l = 30;
int x = 0;
int limit;

byte header[] = new byte[4];
byte header_check[] = { 0x06, 0x08, 0x07 };
byte newByte;

boolean hasByte = false;

char bmp[] = new char[4000];
  
PrintWriter file;

char frame[][][] = new char[32][32][3];

void setup() {
  size(32, 32);
  background(245);
  println(width + "x" + height);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[3], 921600);

  println("Waiting for data");
  noStroke();
  
  file = createWriter("frame.csv"); 
}

void draw()
{
}

void saveFrame()
{
  for(int y = 0; y < height; y++)
  {
    for(int x = 0; x < width; x++)
    {
      file.print("{" + (int)frame[y][x][0] + "|" + (int)frame[y][x][1] + "|" +  + (int)frame[y][x][2] + "},"); // Write the coordinate to the file
    }
    file.println();
  }
  file.flush(); // Writes the remaining data to the file
  file.close(); // Finishes the file
  println("Frame has been saved.");
}

void serialEvent(Serial s)
{
  try {
    bmp[x] = s.readChar();
    if (x > 1 && bmp[x-1] == 0xfa && bmp[x] == 0xa1 && x < 4000)
    {
      background(245);
      x = 0;
      int i, p;
      for (i = 0, p = 0; i < (x_l * 3 * y_l); i+=3, p++)
      {
        int y = p % x_l;
        int x = (int)( p / x_l );
        set(x, y, color(bmp[i]));
        fill(color(bmp[i]));
        //rect(x*10,y*10,10,10);
        //char pix[] = {bmp[i], bmp[i+1], bmp[i+2]};
        //frame[y][x] = pix;
        //println("{" + frame[y][x][0] + "|" + frame[y][x][1] + "|" +  + frame[y][x][2] + "}");
      }
      x = 0;
      s.clear();
    }
    x++;
  }
  catch(Exception e) {
    //println("error");
  }
  if ( x > 4000)
  {
    x = 0;
    s.clear();
  }
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
}