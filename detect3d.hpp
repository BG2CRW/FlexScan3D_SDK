//预处理：滤波+canny
#ifndef DETECT3D_HPP
#define DETECT3D_HPP
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

class detect3d
{
public:
	void makeMask(cv::Mat depthImage, cv::Mat erodeBinary, cv::Mat silk2D,int threshold, int erodeTimes);
	int check3d(cv::Mat depthImage, cv::Mat silk2D);
	void findModel(cv::Mat depthImage,string path,Point* matchLocation,float threshold);
	int errorReport(cv::Mat imgdepthVert, cv::Mat imgdepthHor, cv::Mat silk2D);
	void FCNImge(cv::Mat imgFCN);
};
#endif
