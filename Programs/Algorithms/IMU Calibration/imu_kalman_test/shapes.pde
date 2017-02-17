void drawText()
{
  textFont(font, 20);
  //float temp_decoded = 35.0 + ((float) (temp + 13200)) / 280;
  //text("temp:\n" + temp_decoded + " C", 350, 250);
  text("Accel (g):\n" + Accel[0] + "\n" + Accel[1] + "\n" + Accel[2], 20, 50);
  text("Gyro (deg/s):\n" + Gyro[0] + "\n" + Gyro[1] + "\n" + Gyro[2], 220, 50);
  text("Mag (gauss):\n" + Mag[0] + "\n" + Mag[1] + "\n" + Mag[2], 420, 50);
  text("RwEst :\n" + RwEst[0] + "\n" + RwEst[1] + "\n" + RwEst[2], 20, 180);
}

void drawAxes()
{
  // display axes
  pushMatrix();
  translate(450, 250, 0);
  stroke(#ffffff);
  line(0, 0, 0, 1*axis_scale, 0, 0);
  line(0, 0, 0, 0, -1*axis_scale, 0);
  line(0, 0, 0, 0, 0, 1*axis_scale);
  stroke(#ffaa00);
  line(0, 0, 0, RwEst[0]*axis_scale, -RwEst[1]*axis_scale, RwEst[2]*axis_scale);
  stroke(#ff0000);
  line(0, 0, 0, -cos(RwEst[2])*axis_scale, sin(RwEst[2])*axis_scale, 0);
  stroke(#ffaaaa);
  noFill();
  ellipse(0, 0, 2*axis_scale, 2*axis_scale);
  text("N", -5, -axis_scale);
  text("S", -5, 12+axis_scale);
  text("E", axis_scale, 5);
  text("W", -12-axis_scale, 5);
  popMatrix();
}

void buildBoxShape()
{
  //box(60, 10, 40);
  noStroke();
  beginShape(QUADS);

  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-30, -5, 20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);

  //Z-
  fill(#0000ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, 5, -20);
  vertex(-30, 5, -20);

  //X-
  fill(#ff0000);
  vertex(-30, -5, -20);
  vertex(-30, -5, 20);
  vertex(-30, 5, 20);
  vertex(-30, 5, -20);

  //X+
  fill(#ffff00);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(30, 5, 20);
  vertex(30, 5, -20);

  //Y-
  fill(#ff00ff);
  vertex(-30, -5, -20);
  vertex(30, -5, -20);
  vertex(30, -5, 20);
  vertex(-30, -5, 20);

  //Y+
  fill(#00ffff);
  vertex(-30, 5, -20);
  vertex(30, 5, -20);
  vertex(30, 5, 20);
  vertex(-30, 5, 20);

  endShape();
}

void buildPenShape() 
{
  int pen_w = 5;
  int pen_h = 5;
  int pen_l = 50;
  noStroke();
  beginShape(QUADS);

  //Z+ (to the drawing area)
  fill(#00ff00);
  vertex(-pen_w, -pen_h, pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);

  //Z-
  fill(#0000ff);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, pen_h, -pen_l);
  vertex(-pen_w, pen_h, -pen_l);

  //X-
  fill(#ff0000);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(-pen_w, -pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, -pen_l);

  //X+
  fill(#ffff00);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(pen_w, pen_h, -pen_l);

  //Y-
  fill(#ff00ff);
  vertex(-pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, -pen_l);
  vertex(pen_w, -pen_h, pen_l);
  vertex(-pen_w, -pen_h, pen_l);

  //Y+
  fill(#00ffff);
  vertex(-pen_w, pen_h, -pen_l);
  vertex(pen_w, pen_h, -pen_l);
  vertex(pen_w, pen_h, pen_l);
  vertex(-pen_w, pen_h, pen_l);

  endShape();
}

void drawCube() {  
  pushMatrix();
  translate(200, 400, 0);
  scale(4, 4, 4);

  rotateY( RwEst[2] + PI/2);
  rotateX(-RwEst[0]);
  rotateZ(-RwEst[1]);


  buildPenShape();
  popMatrix();
  
  pushMatrix();
  translate(IND_AXES_X, IND_AXES_Y, 0);
  rotateX(RwEst[0]);
  buildBoxShape();
  popMatrix();
  
  pushMatrix();
  translate(IND_AXES_X+100, IND_AXES_Y, 0);
  rotateZ(RwEst[1]);
  buildBoxShape();
  popMatrix();
  
  pushMatrix();
  translate(IND_AXES_X+200, IND_AXES_Y, 0);
  rotateY(RwEst[2]);
  buildBoxShape();
  popMatrix();
  
}