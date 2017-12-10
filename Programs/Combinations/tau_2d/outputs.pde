void drawGraphs(OUTPUTS o)
{ 
  int scale = 100, gap = 10, xh = height - 3*(scale + gap), yh = height - 2*(scale + gap), zh = height - (scale + gap);
  float x = 0.0, y = 0.0, z = 0.0;
  switch (o)
  {
  case ACCEL:
    scale = 100;
    x = Accel[0] * scale;
    y = Accel[1] * scale;
    z = Accel[2] * scale;
    break;
  case GYRO:
    scale = 1;
    x = Gyro[0] * scale;
    y = Gyro[1] * scale;
    z = Gyro[2] * scale;
    break;
  case MAG:
    scale = 15;
    x = Mag[0] * scale;
    y = Mag[1] * scale;
    z = Mag[2] * scale;
    break;
  default:
    break;
  }

  println("xPos is " + xPos + " and x is " + x);

  stroke(0, 255, 55);
  line(xPos, xh, xPos, xh - x);
  stroke(0, 127, 127);
  line(xPos, yh, xPos, yh - y);
  stroke(127, 34, 255);
  line(xPos, zh, xPos, zh - z);
  if (xPos >= width) {
    background(0);
    xPos = 0;
  } else if (live) {
    xPos++;
  }
  stroke(0);
}

void drawCompass()
{
  fill(0);
  rect(0, 0, width, 600);
  fill(#ffffff);
  textFont(font, 20);
  //float temp_decoded = 35.0 + ((float) (temp + 13200)) / 280;
  //text("temp:\n" + temp_decoded + " C", 350, 250);
  text("Accel (g):\n" + Accel[0] + "\n" + Accel[1] + "\n" + Accel[2], 20, 50);
  text("Gyro (deg/s):\n" + Gyro[0] + "\n" + Gyro[1] + "\n" + Gyro[2], 220, 50);
  text("Mag (gauss):\n" + Mag[0] + "\n" + Mag[1] + "\n" + Mag[2], 420, 50);
  text("RwEst :\n" + RwEst[0] + "\n" + RwEst[1] + "\n" + RwEst[2], 20, 180);

  pushMatrix();
  translate(450, 250);
  stroke(#ffaa00);
  line(0, 0, RwEst[0]*axis_scale, -RwEst[1]*axis_scale);
  stroke(#ff0000);
  line(0, 0, -cos(RwEst[2])*axis_scale, sin(RwEst[2])*axis_scale);
  stroke(#ffaaaa);
  noFill();
  ellipse(0, 0, 2*axis_scale, 2*axis_scale);
  text("N", -5, -axis_scale);
  text("S", -5, 12+axis_scale);
  text("E", axis_scale, 5);
  text("W", -12-axis_scale, 5);
  popMatrix();
}

void drawTilts()
{
  stroke(0);

  pushMatrix();
  translate(IND_AXES_X, IND_AXES_Y);
  fill(#ff0000);
  rotate(pitch.value);
  rect(-IND_AXES_W/2, -IND_AXES_H/2, IND_AXES_W, IND_AXES_H);
  popMatrix();

  pushMatrix();
  translate(IND_AXES_X+100, IND_AXES_Y);
  fill(#00ff00);
  rotate(roll.value);
  rect(-IND_AXES_W/2, -IND_AXES_H/2, IND_AXES_W, IND_AXES_H);
  popMatrix();

  pushMatrix();
  translate(IND_AXES_X+200, IND_AXES_Y);
  fill(#0000ff);
  rotate(yaw.value);
  rect(-IND_AXES_W/2, -IND_AXES_H/2, IND_AXES_W, IND_AXES_H);
  popMatrix();
}

void drawPosition()
{
}