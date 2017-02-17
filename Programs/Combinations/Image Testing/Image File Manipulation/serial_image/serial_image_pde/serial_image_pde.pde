import processing.serial.*; 

Serial myPort; 
PrintWriter output; 

int MAX_CD = 20;

int RES = 10;

int pixel = 0; 
int x_l = 10; 
int y_l = 30; 
int x = 0; 
int limit; 

byte header[] = new byte[4]; 
byte header_check[] = { 0x06, 0x08, 0x07 }; 
byte newByte; 

boolean hasByte = false; 

boolean wait = false;

int bmp[] = new int[4000]; 

void setup() { 
  size(100, 300); 
  background(0); 
  println(width + "x" + height); 
  println( Serial.list() ); 
  myPort = new Serial( this, Serial.list()[3], 921600); 

  println("Waiting for data"); 
  stroke(245);
  fill(0, 255, 0);
} 

void draw() 
{
} 

void getData()
{
  //if (x < 300) return;
  int max_b = x-MAX_CD;
  //println(max_b);
  int i;
  for (i = x-2; i >= max_b; i--)
  {
    if (bmp[i] == 0xee)
    {
      wait = true;
      int n = bmp[i+1];
      background(245); 
      for (int j = 0; j < n; j++)
      {
        int k = i + 2 + j*6;
        int x = int( bmp[k] << 8 ) + int(bmp[k+1]);
        int y = int(bmp[k+2] << 8) + int(bmp[k+3]);
        int m = int(bmp[k+4] << 8) + int(bmp[k+5]);
        println(j+1 + ": " + m + "(" + x + ", " + y + ")");
        //set(x, y, color(255, 0, 0));
        drawX(x, y, color(255, 0, 0));
      }
      if (n > 0) 
      {
        text(n + " centroids found.", 100, 100);
        println(n + " centroids found.");
      }
      wait = false;
      return;
    }
  }
}

void serialEvent(Serial s) 
{ 
  if (!wait)
  {
    try 
    { 
      bmp[x] = s.read(); 
      if (x > 1 && bmp[x-1] == 0xab && bmp[x] == 0x34) 
      { 
        background(245); 

        //int i, p; 
        //for (i = 0, p = 0; i < (x_l * 2 * y_l); i+=2, p++) 
        //{ 
        //  int x = p % x_l; 
        //  int y = (int)( p / x_l ); 
        //  int a = bmp[i];
        //  int b = bmp[i+1];
        //  int br = (a + b)/2;
        //  //set(x, y, color(br));
        //  drawSquare(x, y, color(br));
        //} 
        getData();
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
} 

void drawSquare( int x, int y, color b )
{
  int xl = width - RES - x*RES; 
  int yl = height - RES - y*RES; 
  for (int i = xl; i < xl + RES; i++ )
  {
    for (int j = yl; j < yl + RES; j++ )
    {
      set(i, j, b);
    }
  }
}

void drawX( int x, int y, color b )
{
  drawSquare(x, y, b);
  drawSquare(x-1, y, b);
  drawSquare(x+1, y, b);
  drawSquare(x, y-1, b);
  drawSquare(x, y+1, b);
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
    //while (true);
  } 
  background(245); 
  println("New x is " + x_l + " and y is " + y_l);
}

void mousePressed()
{
  color t = get(mouseX, mouseY);
  println("Color is " + brightness(t));
}