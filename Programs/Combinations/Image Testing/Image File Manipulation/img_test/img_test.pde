import processing.serial.*;

Serial myPort;
PrintWriter output;

int pixel = 0;
int x = 54;
int y = 0;
int limit;

byte header[] = new byte[4];
byte header_check[] = { 0x06, 0x08, 0x07 };
byte newByte;

boolean hasByte = false;

int bmp_header[] =
  { 0x42, 0x4d, // BMP type signature
  0x78, 0x05, 00, 00, // Image size (unreliable)
  00, 00, // Reserved
  00, 00, // Reserved
  0x36, 00, 00, 00, // Image offset
  0x28, 00, 00, 00, // Size of BITMAPINFOHEADER structure (must be 0x28)
  0x38, 00, 00, 00, // Image width
  0x19, 00, 00, 00, // Image Height
  0x01, 00, // Number of planes (must be 1)
  0x08, 00, // Number of bits per pixel (1, 4, 8, or 0x18)
  00, 00, 00, 00, // Compression type
  0xae, 0x05, 00, 00, // Size of image data in bytes + padding
  00, 00, 00, 00, // Horizontal resolution (unreliable)
  00, 00, 00, 00, // Vertical resolution (unreliable)
  00, 00, 00, 00, // Number of colors in image or zero
  00, 00, 00, 00  // Number of important colors or zero
};

int bmp[] = new int[1454];

void setup() {
  size(96, 32);
  background(245);
  println(width + "x" + height);
  println( Serial.list() );
  myPort = new Serial( this, Serial.list()[3], 500000);

  limit = width * height;

  output = createWriter("img_data.bmp");
  for (int i = 0; i < 54; i++)
  {
    bmp[i] = bmp_header[i];
    print(hex(bmp_header[i],2) + " ");
  }
  println("Header written");
}

void draw()
{
  if (x >= 1450)
  {
    int i;
    for (i = 0; i < 1450; i++)
    {
      print(hex(bmp[i],2) + " ");
    }
    for (i = 0; i < 4; i++)
    {
      print(hex(0,2) + " ");
    }
    println("\r\nDone");
    exit();
  }
}

void serialEvent(Serial p)
{
  int newChar = p.read();
  bmp[x] = newChar;
  x++;
  println(x + ":" + newChar);
}



void keyPressed() {
  myPort.write(key);
  print(key);
}