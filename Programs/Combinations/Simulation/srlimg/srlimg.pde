import processing.serial.*; 
Serial serial; 

int image_width = 64;
int image_height = 64;

String serial_in;
byte line[] = new byte[image_width];
int index = 0;

void setup() {  
  size(640, 640);
  try
  {
    serial = new Serial(this, Serial.list()[2], 115200);
  }
  catch(Exception e)
  {
    print("Serial port not available.");
    serial = new Serial(this, Serial.list()[0], 115200);
  }
  serial.bufferUntil('\n'); // Buffer until line feed
}

void draw()
{ 
  background(245);
}

void serialEvent (Serial serial) {
  for(int i = 0; i < image_width; i++)
  {
    serial_in = serial.readStringUntil('\t');
    line[i] = byte(serial_in);
  }
  
  serial.clear();
}