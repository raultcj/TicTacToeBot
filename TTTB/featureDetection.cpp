#include "opencv2\imgproc.hpp"
#include <math.h>

using namespace cv;
using namespace std;

vector<Vec3f> getCircles(Mat source) {
	vector<Vec3f> circles;
	Mat gray;

	cvtColor(source, gray, COLOR_BGR2GRAY);
	GaussianBlur(gray, gray, Size(9, 9), 2, 2);
	HoughCircles(gray, circles, HOUGH_GRADIENT, 2, gray.rows / 4, 200, 100, 0, 200);

	return circles;
}

vector< vector<Point> > getCrosses(Mat source, Mat templ) {
	int cols = source.cols - templ.cols + 1;
	int rows = source.rows - templ.rows + 1;

	Mat result(cols, rows, CV_32FC1);
	matchTemplate(source, templ, result, CV_TM_CCOEFF);
	normalize(result, result, 0, 255.0, NORM_MINMAX, CV_8UC1, Mat());

	Mat resultMask;
	threshold(result, resultMask, 180.0, 255.0, THRESH_BINARY);

	Mat temp = resultMask.clone();
	vector < vector<Point> > crosses;
	findContours(temp, crosses, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(templ.cols / 2, templ.rows / 2));

	return crosses;
}