//Flatulence check
#ifndef FLATULENCE_HPP
#define FLATULENCE_HPP
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class flatulence
{
public:
	double stdDevValue;
	int flatulenceCheck(cv::Mat depthImage,float mean,float flaTh,int whiteTh,int whiteNum);
};
#endif
