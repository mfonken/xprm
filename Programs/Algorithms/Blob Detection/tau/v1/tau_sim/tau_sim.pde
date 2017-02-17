BufferedReader file, input;

int xyscale = 10;
int zscale  = 10;

float rot_x, rot_y, rot_z;

int xi, yi, xm, ym;

int in[][];
float map[][];

void setup()
{
  size(1000, 700, P3D);
  background(15);

  String line;
/************************ Test file ************************/
  input = createReader("../test.txt"); 

  try {
    line = input.readLine();
    int lineInt[] = int(split(line, " "));
    xi = lineInt[0];
    yi = lineInt[1];
  } 
  catch (IOException e) {
    e.printStackTrace();
    xi = 1;
    yi = 1;
  }
  in = new int[yi][xi];
  try {
    for (int i = 0; i < yi; i++)
    {
      line = input.readLine();
      int lineInt[] = int(split(line, " "));
      for (int j = 0; j < xi; j++)
      {
        in[i][j] = lineInt[j];
      }
    }
  }
  catch (IOException e) {
    e.printStackTrace();
  }
  
  for (int i = 0; i < yi; i++)
  {
    for (int j = 0; j < xi; j++)
    {
      print(in[i][j] + " ");
    }
    println();
  }
  
/************************ Output file ************************/
  file = createReader("../output.txt"); 
  try {
    line = file.readLine();
    int lineInt[] = int(split(line, " "));
    xm = lineInt[0];
    ym = lineInt[1];
  } 
  catch (IOException e) {
    e.printStackTrace();
    xm = 1;
    ym = 1;
  }
  println("Drawing " + xm + "x" + ym + " map.");
  map = new float[ym][xm];
  try {
    for (int i = 0; i < ym; i++)
    {
      line = file.readLine();
      float lineInt[] = float(split(line, " "));
      for (int j = 0; j < xm; j++)
      {
        map[i][j] = lineInt[j] * zscale;
      }
    }
  }
  catch (IOException e) {
    e.printStackTrace();
  }
  for (int i = 0; i < ym; i++)
  {
    for (int j = 0; j < xm; j++)
    {
      print(map[i][j] + " ");
    }
    println();
  }
}

void draw()
{
  background(15);
  drawMaps();
}

void drawMaps()
{
  pushMatrix();
  translate(width/2, height/2, 0);
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
  
  popMatrix();
}

void drawInput()
{
  stroke(200);
  for (int i = 0; i < yi-1; i++)
  {
    int is = i * xyscale;
    for (int j = 0; j < xi-1; j++)
    {
      int js = j * xyscale;
      if(in[i][j] == 0) noFill();
      else fill(0,255,255);
      rect(js, is, xyscale, xyscale);
    }
  }
}


void drawMap()
{
  stroke(255,255,0);
  int xscale = xyscale * xi/xm;
  int yscale = xyscale * yi/ym;
  for (int i = 0; i < ym-1; i++)
  {

    int is = i * yscale;
    for (int j = 0; j < xm-1; j++)
    {
      int js = j * xscale;
      line(js, is, map[i][j], js+xscale, is, map[i][j+1]);
      line(js, is, map[i][j], js, is+yscale, map[i+1][j]);
    }
  }
  
  for (int i = 0; i < ym-1; i++)
  {
    int is = i * yscale;
    int js = (xm-1) * xscale;
    line(js, is, map[i][xm-1], js, is+yscale, map[i+1][xm-1]);
  }
  for (int j = 0; j < xm-1; j++)
  {
    int is = (ym-1) * yscale;
    int js = j * xscale;
    line(js, is, map[ym-1][j], js+xscale, is, map[ym-1][j+1]);
  }
}