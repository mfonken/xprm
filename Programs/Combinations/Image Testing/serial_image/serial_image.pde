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

int bmp[] = new int[4000];

void setup() {
  size(25, 32);
  background(245);
  println(width + "x" + height);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[3], 921600);

  println("Waiting for data");
  noStroke();
}

void draw()
{
}

void serialEvent(Serial s)
{
  try {
    bmp[x] = s.read();
    if (x > 1 && bmp[x-1] == 0xfa && bmp[x] == 0xa1)
    {
      background(245);
      x = 0;
      int i, p;
      for (i = 0, p = 0; i < (x_l * 3 * y_l); i+=3, p++)
      {
        int y = p % x_l;
        int x = (int)( p / x_l );
        set(x, y, color(bmp[i], bmp[i+1], bmp[i+2]));
      }
      x = 0;
      s.clear();
    }
    x++;
  }
  catch(Exception e) {
    println("error");
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
  default:
    while (true);
  }
  background(245);
  println("New x is " + x_l + " and y is " + y_l);
}