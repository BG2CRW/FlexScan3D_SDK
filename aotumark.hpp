//凹凸痕检测
#ifndef AOTUMARK_HPP
#define AOTUMARK_HPP
#include "opencv2/opencv.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;

class aotumark
{
public:
	vector<cv::Point2d> aotumarkCheck(cv::Mat image);
};
#endif
