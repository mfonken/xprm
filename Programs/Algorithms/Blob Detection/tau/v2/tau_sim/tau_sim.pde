BufferedReader file;

float xyscale = 1.5;
float zscale = 0.05;

float xscale, yscale;

float rot_x, rot_y, rot_z;

int xi, yi, xm, ym;

float in[][];
int map[][];

boolean ready  = false;
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

  selectInput("Select a file to process:", "fileSelected");
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
        if(c != 0) c = 1;
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
  println("Scale is " + s + " ( " + yi + " / " + ym + " )");
  xscale = xyscale * s;
  yscale = xyscale * s;
    
  ready = true;
}

void draw()
{
  if(ready)
  {
    background(15);
    drawMaps();
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
  drawInput();
  drawMap();
  drawDensityMaps();
  drawCentroids();
  popMatrix();
}

void drawCentroids()
{
  for(int i = 0; i < ypn; i++ )
  {
    float ypc = yp[i] * yscale;
    for(int j = 0; j < xpn; j++ )
    {
      stroke(255,0,0);
      float xpc = xp[j] * xscale;
      translate(xpc, ypc, map[yp[i]][xp[j]]);
      sphere(3);
      translate(-xpc, -ypc, -map[yp[i]][xp[j]]);
    }
  }
}

void drawInput()
{
  stroke(200);
  strokeWeight(1);
  rect(0,0, xi*xyscale, yi*xyscale);
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
  stroke(255,100,0);
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
      float js = j * xscale;
      line(js, is, map[i][j], js+xscale, is, map[i][j+1]);
      line(js, is, map[i][j], js, is+yscale, map[i+1][j]);
    }
  }
}