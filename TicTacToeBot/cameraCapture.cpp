#include "opencv2/videoio.hpp"

using namespace cv;

Mat cameraCapture() {
  VideoCapture cap(0);
  Mat source;

  cap >> source;

  return source;
}