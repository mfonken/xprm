void setup() {
  size(640, 360, P3D);
  noStroke();
}

void draw() {
  lights();
  background(15);
  
  float cameraY = height/2.0;
  float fov = PI/4;
  float cameraZ = cameraY / tan(fov / 2.0);
  float aspect = float(width)/float(height);
  perspective(fov, aspect, cameraZ/10.0, cameraZ*10.0);
  
  translate(width/2, height/2, 0);
  if (mousePressed) {
    translate(mouseX - float(width)/2, 0, float(height)/2 - mouseY);
  }
  rotateX(-PI/6);
  rotateY( mouseX/float(width) * PI);
  
  
  fill(245);
  box(150, 5, 200);
  
  fill(245, 50, 0);
  translate(0, 0, -50);
  sphere(15);
  
  translate(0, 0, 100);
  sphere(15);
}