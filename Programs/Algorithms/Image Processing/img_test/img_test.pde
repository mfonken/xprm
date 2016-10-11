void setup() 
{
  size(64,64);
  background(250);
}

void draw()
{
}

void keyPressed()
{
  switch(key)
  {
    case 'r':
      for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
          color c = color(random(0xff), random(0xff), random(0xff));
          set(i, j, c);
        }
      }
      break;
    default:
      background(250);
      break;
  }
  save("img.bmp");
}