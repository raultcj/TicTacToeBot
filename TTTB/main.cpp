#include "featureDetection.h"
#include "serialConnection.h"
#include "opencv2/highgui.hpp"
#include <iostream>
#include <queue>

using namespace std;
using namespace cv;

//Debug variable
bool debug = true;

const char *source_window = "Source Image";
const char *result_window = "Visualization";

//Path for images used in debugging/testing.
const string pathTempl = "C:/Users/Raul Juarez/Pictures/images/cross-template.png";
const string pathDebug = "C:/Users/Raul Juarez/Pictures/images/test2.png";

//These values define the player and the AI's symbol. (Either x or o).
string player;
string pc;

//The default camera is 0.
int cameraID = 0;
//These variables define the position of each piece in the image captured.
int __x1, __x2, __x3;
int __y1, __y2, __y3;

Mat source, vis, templ;

//Creating a Piece object with its x and y position, along with its value.
class Piece {
public:
	string print();
	int getX();
	int getY();
	Piece();
	Piece(double posX, double posY, string value);
private:
	double x;
	double y;
	string val;
};

//Returns the Piece's symbol.
string Piece::print()
{
	return val;
}

//Returns position in X.
int Piece::getX()
{
	return x;
}

//Returns position in Y.
int Piece::getY()
{
	return y;
}

//Default value for symbol is - or null.
Piece::Piece()
{
	val = "-";
}

//Constructor for complete Piece object.
Piece::Piece(double posX, double posY, string value)
{
	x = posX;
	y = posY;
	val = value;
}

//Create a 3x3 board.
Piece board[3][3];

//Method used to add a Piece to the 3x3 board. Requieres a Point (x and y values) and its symbol.
void addPiece(Point point, string value) {
	if (point.x > 0 && point.x < __x1) {
		if (point.y > 0 && point.y < __y1) {
			board[0][0] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y1 && point.y < __y2) {
			board[0][1] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y2 && point.y < __y3) {
			board[0][2] = Piece(point.x, point.y, value);
		}
	}
	else if (point.x > __x1 && point.x < __x2) {
		if (point.y > 0 && point.y < __y1) {
			board[1][0] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y1 && point.y < __y2) {
			board[1][1] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y2 && point.y < __y3) {
			board[1][2] = Piece(point.x, point.y, value);
		}
	}
	else if (point.x > __x2 && point.x < __x3) {
		if (point.y > 0 && point.y < __y1) {
			board[2][0] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y1 && point.y < __y2) {
			board[2][1] = Piece(point.x, point.y, value);
		}
		else if (point.y > __y2 && point.y < __y3) {
			board[2][2] = Piece(point.x, point.y, value);
		}
	}
	//What the fuck went wrong?
	else {
		cout << "Something horrible happened." << endl;
	}
}

//Calculate next move to send to Arduino.
void getNextMove() {
	int rand1, rand2, next;

	while (true) {
		//Random number from 0 to 2.
		rand1 = rand() % 3;
		rand2 = rand() % 3;

		if (board[rand1][rand2].print() == "-") {
			//Next Move is defined by the position the which will be played next.
			//|1|2|3|
			//|4|5|6|
			//|7|8|9|
			next = rand1 + (rand2 * 3) + 1;
			connect(next);
			return;
		}
	}
}

//Camera capture method.
Mat cameraCapture() {
	VideoCapture cap(cameraID);
	Mat capture;

	cap >> capture;

	return capture;
}

int main(int argc, char **argv) {
	cout << "Starting program..." << endl;

	templ = imread(pathTempl, 1);

	if (templ.empty()) {
		cout << "Failed to load cross template." << endl;
		return -1;
	}

	cout << "Enter player's choice. (x or o)" << endl;
	cin >> player;

	if (player.compare("x") == 0 || player.compare("X") == 0) {
		pc = "o";
		player = "x";
	}
	else if (player.compare("o") == 0 || player.compare("O") == 0) {
		pc = "x";
		player = "o";
	}
	else {
		cout << "Invalid input detected, assining 'o' to the player and 'x' to the PC." << endl;
		pc = "x";
		player = "o";
	}

	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	namedWindow(result_window, CV_WINDOW_AUTOSIZE);

	cout << "Getting image from camera..." << endl;

	while (waitKey(33) != 27) {
		if (debug) {
			source = imread(pathDebug);
			source.copyTo(vis);
		}
		else {
			source = cameraCapture();
		}

		if (source.empty()) {
			cout << "Captured empty frame, verify camera input or reconfigure." << endl;
			return -1;
		}

		__x1 = source.cols / 3 * 1;
		__x2 = source.cols / 3 * 2;
		__x3 = source.cols / 3 * 3;

		__y1 = source.rows / 3 * 1;
		__y2 = source.rows / 3 * 2;
		__y3 = source.rows / 3 * 3;

		vector<Vec3f> circles = getCircles(source);
		vector< vector<Point> > crosses = getCrosses(source, templ);

		cout << "Found " << circles.size() << " circles." << endl;
		cout << "Found " << crosses.size() << " crosses." << endl;

		for (size_t i = 0; i < circles.size(); i++) {
			Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
			int radius = cvRound(circles[i][2]);

			//Draw circle
			circle(vis, center, 3, Scalar(0, 255, 0), -1, 8, 0);

			addPiece(center, "o");
		}

		vector< vector<Point> >::iterator i;

		for (i = crosses.begin(); i != crosses.end(); i++) {
			Moments m = moments(*i, false);
			Point centroid(m.m10 / m.m00, m.m01 / m.m00);
			circle(vis, centroid, 3, Scalar(0, 0, 255), -1, 8, 0);

			addPiece(centroid, "x");
			//cout << "Added " << centroid.x << "/" << centroid.y << endl;
		}

		imshow(source_window, source);
		imshow(result_window, vis);

		cout << board[0][0].print() << board[1][0].print() << board[2][0].print() << endl;
		cout << board[0][1].print() << board[1][1].print() << board[2][1].print() << endl;
		cout << board[0][2].print() << board[1][2].print() << board[2][2].print() << endl;

		waitKey(0);
	}

	destroyAllWindows();
	return 0;
}