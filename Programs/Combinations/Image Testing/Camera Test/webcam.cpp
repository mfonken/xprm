/*
* File:   main.cpp
* Author: sagar
*
* Created on 10 September, 2012, 7:48 PM
*/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

int main() {
  VideoCapture cam0(0);   //0 is the id of video device.0 if you have only one camera.
  VideoCapture cam1(1);

  if (!cam0.isOpened()) { //check if video device has been initialised
    cout << "cannot open camera 0";
  }
  if (!cam0.isOpened()) { //check if video device has been initialised
    cout << "cannot open camera 1";
  }

  //unconditional loop
  while (true) {
    Mat cam0Frame, cam1Frame;
    cam0.read(cam0Frame);
    cam1.read(cam1Frame);
    imshow("cam0", cam0Frame);
    imshow("cam1", cam1Frame);
    if (waitKey(30) >= 0)
    break;
  }
  return 0;
}
