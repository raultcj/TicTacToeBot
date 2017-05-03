#include "cameraCapture.h"
#include "featureDetection.h"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <queue>

using namespace std;
using namespace cv;

bool debug = true;
const char *source_window = "Source Image";
const char *result_window = "Visualization";

Mat source;
Mat vis;
Mat templ;

class Piece {
public:
  double posX;
  double posY;
  // Should be "", "x" or "o"
  string value;
};

Piece board[3][3];

int main(int argc, char **argv) {
  cout << "Starting program..." << endl;

  const string pathTempl =
      "/home/rj/Documents/TicTacToeBot/images/cross-template.png";
  templ = imread(pathTempl, 1);

  if (templ.empty()) {
    cout << "Failed to load cross-template.png" << endl;
  }

  namedWindow(source_window, CV_WINDOW_AUTOSIZE);
  namedWindow(result_window, CV_WINDOW_AUTOSIZE);

  cout << "Getting image from camera..." << endl;

  while (waitKey(33) != 27) {
    if (debug) {
      source = imread("/home/rj/Documents/TicTacToeBot/images/test2.png", 1);
      source.copyTo(vis);
    } else {
      source = cameraCapture();
    }

    if (source.empty()) {
      cout << "Captured empty frame, verify camera input or reconfigure."
           << endl;
      return (-1);
    }

    vector<Vec3f> circles = getCircles(source);
    vector<vector<Point> > crosses = getCrosses(source, templ);

    cout << "Found " << circles.size() << " circles" << endl;
    cout << "Found " << crosses.size() << " crosses" << endl;

    for (size_t i = 0; i < circles.size(); i++) {
      Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
      int radius = cvRound(circles[i][2]);
      // Draw circle center
      circle(vis, center, 3, Scalar(0, 255, 0), -1, 8, 0);
      // Draw circle outline
      // circle(vis, center, radius, Scalar(0, 255, 0), -1, 8, 0);

      /*cout << "!!" << endl;
      cout << center.x << endl;
      cout << center.y << endl;*/
    }

    vector<vector<Point> >::iterator i;

    for (i = crosses.begin(); i != crosses.end(); i++) {
      Moments m = moments(*i, false);
      Point centroid(m.m10 / m.m00, m.m01 / m.m00);
      circle(vis, centroid, 3, Scalar(0, 0, 255), -1, 8, 0);

      /*cout << "//" << endl;
      cout << centroid.x << endl;
      cout << centroid.y << endl;*/
    }

    imshow(source_window, source);
    imshow(result_window, vis);

    //break;
  }

  destroyAllWindows();
  return (0);
}