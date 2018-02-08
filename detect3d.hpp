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
	int check3d(cv::Mat depthImage, cv::Mat silk2D, vector<vector<Point>>& contoursAl, vector<vector<Point>>& contoursLiquid);
	void findModel(cv::Mat depthImage,string path,Point* matchLocation,float threshold);
	int errorReport(cv::Mat imgdepthVert, cv::Mat imgdepthHor, cv::Mat silk2D, vector<vector<Point>>& contoursAl, vector<vector<Point>>& contoursLiquid);
	int findlogo(cv::Mat image3D, int face, Point *matchLocation_3Dapple);
	void judge(cv::Mat src, cv::Mat image, int* x, int* y, int* xmax, int* ymax, int* area, int* length, int *depthmax,int* depthmin, int& num);

};
#endif
