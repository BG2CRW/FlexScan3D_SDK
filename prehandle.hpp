//预处理：滤波+canny
#ifndef PREHANDLE_HPP
#define PREHANDLE_HPP
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class prehandle
{
public:
	void makeMask(cv::Mat depthImage,cv::Mat erodeBinary,int threshold,int erodeTimes,int* classify,Point* matchLocation);
	int prejudge(cv::Mat depthImage);
	void findModel(cv::Mat depthImage,string path,Point* matchLocation,float threshold);
	void judgeInv(int* classify,int* inv,int* pos,Mat depthImage,Point* matchLocation);
};
#endif
