import processing.serial.*;

Serial myPort;
OutputStream output;


void setup() {

  size(320, 240);

  //println( Serial.list() );
  myPort = new Serial( this, Serial.list()[0], 115200);
  myPort.clear();

  output = createOutput("pic01.png");
}


void draw() {
  try {
    while ( myPort.available () > 0 ) {
      output.write(myPort.read());
    }
  } 
  catch (IOException e) {
    e.printStackTrace();
  }
}


void keyPressed() {

  try { 
    output.flush();  // Writes the remaining data to the file
    output.close();  // Finishes the file
  } 

  catch (IOException e) {
    e.printStackTrace();
  }
}