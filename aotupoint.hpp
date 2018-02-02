//凹凸点检测
#ifndef AOTUPOINT_HPP
#define AOTUPOINT_HPP
#include "opencv2/opencv.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;

class aotupoint
{
public:
	vector<cv::Point2d> aotupointCheck(cv::Mat image);
};
#endif
