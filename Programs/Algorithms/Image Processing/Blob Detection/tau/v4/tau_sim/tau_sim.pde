<<<<<<< Updated upstream
import processing.video.*;

Capture cam;

BufferedReader file;

int uni_w = 250;
int uni_h = 250;

float xyscale = 1.5;
float zscale = 0.05;

float xscale, yscale;

float rot_x, rot_y, rot_z;

int xi, yi, xm, ym;

float cam_thresh = 0.9;

float in[][];
int map[][];

boolean debug = false;

int max_height = 110;

boolean ready = false;
void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
    exit();
  } else {
    println("User selected " + selection.getAbsolutePath());
    file = createReader(selection.getAbsolutePath());
    getMapFromFile();
  }
}

void setup()
{
  size(1000, 700, P3D);
  background(15);
  xi = uni_w;
  yi = uni_h;
  
  in = new float[yi][xi];
  //selectInput("Select a file to process:", "fileSelected");

  String[] cameras = Capture.list();

  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  } else {
    println("Available cameras:");
    for (int i = 0; i < cameras.length; i++) {
      println(cameras[i]);
    }

    // The camera can be initialized directly using an 
    // element from the array returned by list():
    cam = new Capture(this, cameras[0]);
    cam.start();
  }
  
  while(cam.width == 0)
  {
    if (cam.available() == true) {
      cam.read();  
      if(debug)
        println("Camera is " + cam.width + "x" + cam.height);
    }
    delay(100);
  }
  
  ready = true;
}

void getMapFromFile()
{
  println("File selected.");

  /************************ Output file ************************/

  try {
    file.skip(18);
    xi = 200;
    yi = 200;
    println("Drawing " + xi + "x" + yi + " map.");
    file.skip(54);
  } 
  catch (IOException e) {
    e.printStackTrace();
    xi = 1;
    yi = 1;
  }


  in = new float[yi][xi];
  try {
    for (int i = 0; i < yi; i++)
    {
      for (int j = 0; j < xi; j++)
      {
        int c = file.read();
        file.skip(3);
        if (c != 0) c = 1;
        else c = 0;
        in[i][j] = (float)c * zscale;
      }
    }
  }
  catch (IOException e) {
    e.printStackTrace();
  }
  processImage();

  float s = (float)yi/(float)ym;
  if(debug)
    println("Scale is " + s + " ( " + yi + " / " + ym + " )");
  xscale = xyscale * s;
  yscale = xyscale * s;

  ready = true;
}

void draw()
{
  if (ready)
  {
    background(15);
    drawMaps();
  }
}

void drawCam()
{
  if (cam.available() == true) {
    cam.read();
  }
  
  cam.filter(THRESHOLD, cam_thresh);
  
  float cam_xscale = cam.width / xi;
  float cam_yscale = cam.height / yi;
  
  for(int x = 0; x < xi; x++ ) {
    for( int y = 0; y < yi; y++ ) {
      int c = (int)red(cam.get((int)(x*cam_xscale), (int)(y*cam_yscale)));
      if (c != 0) c = 1;
      in[y][x] = (float)c * zscale;
    }
  }
  processImage();

  float s = (float)yi/(float)ym;
  if(debug)
    println("Scale is " + s + " ( " + yi + " / " + ym + " )");
  xscale = xyscale * s;
  yscale = xyscale * s;

  //drawCamImage(cam);
}

void drawCamImage(PImage cam)
{
  beginShape();
  noStroke();
  texture(cam);
  vertex(0, 0, 0, 0, 0);
  vertex( xi, 0, 0, cam.width, 0);
  vertex( xi,  yi, 0, cam.width, cam.height);
  vertex( 0, yi, 0, 0, cam.height);
  endShape();
}

void drawCentroids()
{
  int max = 0;
  int max_x = 0;
  int max_y = 0;
  stroke(0,0,255);
  for (int i = 0; i < ypn; i++ )
  {
    for (int j = 0; j < xpn; j++ )
    {
      if(map[yp[i]][xp[j]] > max && yp[i] != 0 && xp[j] != 0) 
      {
        max = map[yp[i]][xp[j]];
        max_x = xp[j];
        max_y = yp[i];
      }
      float xpc = xp[j] * xscale;
      float ypc = yp[i] * yscale;
      translate(xpc, ypc, map[yp[i]][xp[j]]);
      sphere(2);
      translate(-xpc, -ypc, -map[yp[i]][xp[j]]);
    }
  }
  
  println("Peaks found: x-" + xpn + " and y-" + ypn); 
  println("Max is " + max + " at (" + max_x + ", " + max_y + ")");
  
  stroke(150, 0, 255);
  float xpc = max_x * xscale;
  float ypc = max_y * yscale;
  translate(xpc, ypc, max);
  sphere(2);
  translate(-xpc, -ypc, -max);
}

void drawInput()
{
  stroke(200);
  strokeWeight(1);
  rect(0, 0, xi*xyscale, yi*xyscale);
  noStroke();

  for (int i = 0; i < yi-1; i++)
  {
    float is = i * xyscale;
    for (int j = 0; j < xi-1; j++)
    {
      float js = j * xyscale;
      if (in[i][j] == 0) noFill();
      else fill(0, 255, 255);
      rect(js, is, xyscale, xyscale);
    }
  }
}

void drawDensityMaps()
{
  stroke(255, 100, 0);
  strokeWeight(5);
  for (int i = 0; i < ym-1; i++)
  {
    float is = i * yscale;
    float js = (xm-1) * xscale;
    line(js, is, map[i][xm-1], js, is+yscale, map[i+1][xm-1]);
  }
  for (int j = 0; j < xm-1; j++)
  {
    float is = (ym-1) * yscale;
    float js = j * xscale;
    line(js, is, map[ym-1][j], js+xscale, is, map[ym-1][j+1]);
  }
}

void drawMap()
{
  strokeWeight(1);
  for (int i = 0; i < ym-1; i++)
  {
    float is = i * yscale;
    for (int j = 0; j < xm-1; j++)
    {
      stroke(map(map[i][j],0,max_height,150,255), map(map[i][j],0,max_height,255,100), 0);
      float js = j * xscale;
      line(js, is, map[i][j], js+xscale, is, map[i][j+1]);
      line(js, is, map[i][j], js, is+yscale, map[i+1][j]);
    }
  }
}

void drawMaps()
{
  pushMatrix();
  strokeWeight(1);
  translate(width/2, height/4, 0);
  rot_x = mouseY/float(height) * PI;
  rot_z = mouseX/float(width) * PI - PI/4;
  rotateX(rot_x);
  rotateZ(rot_z);
  //rotateY();
  //rotateY(-PI/4);
  stroke(245);
  noFill();
  box(25);  
  stroke(255, 0, 0);
  line(0, 0, 0, 200, 0, 0);
  stroke(0, 255, 0);
  line(0, 0, 0, 0, 200, 0);
  stroke(0, 0, 255);
  line(0, 0, 0, 0, 0, 200);
  
  drawCam();
  drawInput();
  drawMap();
  drawDensityMaps();
  drawCentroids();
  popMatrix();
=======
import processing.video.*;

Capture cam;

BufferedReader file;

int uni_w = 250;
int uni_h = 250;

float xyscale = 1.5;
float zscale = 0.03;

float xscale, yscale;

float rot_x, rot_y, rot_z;

int xi, yi, xm, ym;

float cam_thresh = 0.97;

float in[][];
int map[][];

boolean debug = false;

int max_value = 20;

boolean ready = false;
void fileSelected(File selection) {
  if (selection == null) {
    println("Window was closed or the user hit cancel.");
    exit();
  } else {
    println("User selected " + selection.getAbsolutePath());
    file = createReader(selection.getAbsolutePath());
    getMapFromFile();
  }
}

void setup()
{
  size(1000, 700, P3D);
  background(15);
  xi = uni_w;
  yi = uni_h;
  
  in = new float[yi][xi];
  //selectInput("Select a file to process:", "fileSelected");

  String[] cameras = Capture.list();

  if (cameras.length == 0) {
    println("There are no cameras available for capture.");
    exit();
  } else {
    println("Available cameras:");
    for (int i = 0; i < cameras.length; i++) {
      println(cameras[i]);
    }

    // The camera can be initialized directly using an 
    // element from the array returned by list():
    cam = new Capture(this, cameras[0]);
    cam.start();
  }
  
  while(cam.width == 0)
  {
    if (cam.available() == true) {
      cam.read();  
      if(debug)
        println("Camera is " + cam.width + "x" + cam.height);
    }
    delay(100);
  }
  
  ready = true;
}

void getMapFromFile()
{
  println("File selected.");

  /************************ Output file ************************/

  try {
    file.skip(18);
    xi = 200;
    yi = 200;
    println("Drawing " + xi + "x" + yi + " map.");
    file.skip(54);
  } 
  catch (IOException e) {
    e.printStackTrace();
    xi = 1;
    yi = 1;
  }


  in = new float[yi][xi];
  try {
    for (int i = 0; i < yi; i++)
    {
      for (int j = 0; j < xi; j++)
      {
        int c = file.read();
        file.skip(3);
        if (c != 0) c = 1;
        else c = 0;
        in[i][j] = (float)c * zscale;
      }
    }
  }
  catch (IOException e) {
    e.printStackTrace();
  }
  processImage();

  float s = (float)yi/(float)ym;
  if(debug)
    println("Scale is " + s + " ( " + yi + " / " + ym + " )");
  xscale = xyscale * s;
  yscale = xyscale * s;

  ready = true;
}

void draw()
{
  if (ready)
  {
    background(15);
    drawMaps();
  }
}

void drawCam()
{
  if (cam.available() == true) {
    cam.read();
  }
  
  cam.filter(THRESHOLD, cam_thresh);
  
  float cam_xscale = cam.width / xi;
  float cam_yscale = cam.height / yi;
  
  for(int x = 0; x < xi; x++ ) {
    for( int y = 0; y < yi; y++ ) {
      int c = (int)red(cam.get((int)(x*cam_xscale), (int)(y*cam_yscale)));
      if (c != 0) c = 1;
      in[y][x] = (float)c * zscale;
    }
  }
  processImageWithConvolve();

  float s = (float)yi/(float)ym;
  if(debug)
    println("Scale is " + s + " ( " + yi + " / " + ym + " )");
  xscale = xyscale * s;
  yscale = xyscale * s;

  //drawCamImage(cam);
}

void drawCamImage(PImage cam)
{
  beginShape();
  noStroke();
  texture(cam);
  vertex(0, 0, 0, 0, 0);
  vertex( xi, 0, 0, cam.width, 0);
  vertex( xi,  yi, 0, cam.width, cam.height);
  vertex( 0, yi, 0, 0, cam.height);
  endShape();
}

void drawCentroids()
{
  int max = 0;
  int max_x = 0;
  int max_y = 0;
  stroke(255,0,100);
  for (int i = 0; i < ypn; i++ )
  {
    for (int j = 0; j < xpn; j++ )
    {
      if(map[yp[i]][xp[j]] > max && yp[i] != 0 && xp[j] != 0) 
      {
        max = map[yp[i]][xp[j]];
        max_x = xp[j];
        max_y = yp[i];
      }
      float xpc = xp[j] * xscale;
      float ypc = yp[i] * yscale;
      translate(xpc, ypc, map[yp[i]][xp[j]]);
      sphere(2);
      translate(-xpc, -ypc, -map[yp[i]][xp[j]]);
    }
  }
  
  println("Peaks found: x-" + xpn + " and y-" + ypn); 
  println("Max is " + max + " at (" + max_x + ", " + max_y + ")");
  
  stroke(0, 255, 0);
  float xpc = max_x * xscale;
  float ypc = max_y * yscale;
  translate(xpc, ypc, max);
  sphere(2);
  translate(-xpc, -ypc, -max);
}

void drawInput()
{
  stroke(200);
  strokeWeight(1);
  rect(0, 0, xi*xyscale, yi*xyscale);
  noStroke();

  for (int i = 0; i < yi-1; i++)
  {
    float is = i * xyscale;
    for (int j = 0; j < xi-1; j++)
    {
      float js = j * xyscale;
      if (in[i][j] == 0) noFill();
      else fill(0, 255, 255);
      rect(js, is, xyscale, xyscale);
    }
  }
}

void drawDensityMaps()
{
  stroke(255, 100, 0);
  strokeWeight(5);
  for (int i = 0; i < ym-1; i++)
  {
    float is = i * yscale;
    float js = (xm-1) * xscale;
    line(js, is, map[i][xm-1], js, is+yscale, map[i+1][xm-1]);
  }
  for (int j = 0; j < xm-1; j++)
  {
    float is = (ym-1) * yscale;
    float js = j * xscale;
    line(js, is, map[ym-1][j], js+xscale, is, map[ym-1][j+1]);
  }
}

void drawMap()
{
  stroke(255, 255, 0);
  strokeWeight(1);
  for (int i = 0; i < ym-1; i++)
  {
    float is = i * yscale;
    for (int j = 0; j < xm-1; j++)
    {
      stroke(map(map[i][j],0,max_value,225,255),map(map[i][j],0,max_value,255,225),0);
      float js = j * xscale;
      line(js, is, map[i][j], js+xscale, is, map[i][j+1]);
      line(js, is, map[i][j], js, is+yscale, map[i+1][j]);
    }
  }
}

void drawMaps()
{
  pushMatrix();
  strokeWeight(1);
  translate(width/2, height/4, 0);
  rot_x = mouseY/float(height) * PI;
  rot_z = mouseX/float(width) * PI - PI/4;
  rotateX(rot_x);
  rotateZ(rot_z);
  //rotateY();
  //rotateY(-PI/4);
  stroke(245);
  noFill();
  box(25);  
  stroke(255, 0, 0);
  line(0, 0, 0, 200, 0, 0);
  stroke(0, 255, 0);
  line(0, 0, 0, 0, 200, 0);
  stroke(0, 0, 255);
  line(0, 0, 0, 0, 0, 200);
  
  drawCam();
  drawInput();
  drawMap();
  drawDensityMaps();
  drawCentroids();
  popMatrix();
>>>>>>> Stashed changes
}