#ifndef FEATUREDETECTION_H
#define FEATUREDETECTION_H

#include "opencv2/imgproc.hpp"
#include <math.h>

std::vector<cv::Vec3f> getCircles(cv::Mat source);
std::vector< std::vector<cv::Point> > getCrosses(cv::Mat source, cv::Mat templ);

#endif