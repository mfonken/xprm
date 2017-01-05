import processing.serial.*; 
 
Serial myPort; 
PrintWriter output; 
 
int MAX_CD = 20;
 
int pixel = 0; 
int x_l = 20; 
int y_l = 25; 
int x = 0; 
int limit; 
 
byte header[] = new byte[4]; 
byte header_check[] = { 0x06, 0x08, 0x07 }; 
byte newByte; 
 
boolean hasByte = false; 
 
int bmp[] = new int[4000]; 
 
void setup() { 
  size(32, 27); 
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
 
void getData()
{
  if(x < 1600) return;
  int max_b = x-MAX_CD;
  //println(max_b);
  int i;
  for(i = x-2; i >= max_b; i--)
  {
    if(bmp[i] == 0xee)
    {
      int n = bmp[i+1];
      if(n > 0) println(n + " centroids found.");
      for(int j = 0; j < n; j++)
      {
        int k = i + 2 + j*2;
        int x = width - bmp[k];
        int y = height - bmp[k+1];
        println(j+1 + ": (" + x + ", " + y + ")");
        set(x, y, color(255,0,0));
        set(x-1, y, color(255,0,0));
        set(x+1, y, color(255,0,0));
        set(x, y-1, color(255,0,0));
        set(x, y+1, color(255,0,0));
      }
      return;
    }
  }
}
 
void serialEvent(Serial s) 
{ 
  try { 
    bmp[x] = s.read(); 
    if (x > 1 && bmp[x-1] == 0xab && bmp[x] == 0x34) 
    { 
      background(245); 

      int i, p; 
      for (i = 0, p = 0; i < (x_l * 2 * y_l); i+=2, p++) 
      { 
        int x = width - p % x_l; 
        int y = height - (int)( p / x_l ); 
        int a = bmp[i];
        int b = bmp[i+1];
        int br = (a + b)/2;
        set(x, y, color(br)); 
      } 
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

void mousePressed()
{
  color t = get(mouseX, mouseY);
  println("Color is " + brightness(t));
}