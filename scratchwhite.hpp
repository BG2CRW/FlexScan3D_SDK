//Scratch detection (No Chinese)
#ifndef SCRATCHWHITE_HPP
#define SCRATCHWHITE_HPP

#include <stdio.h>
#include <iostream>     
#include <opencv2/opencv.hpp>  
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/features2d.hpp"
#include "config.hpp"

using namespace cv;
using namespace std;

class scratchwhite
{
private:
	void findModel(cv::Mat depthImage, string path, Point* matchLocation, float threshold);
	cv::Mat drawDetectLines(Mat image, const vector<Vec4i> lines, Scalar color);
public:
	int whitescratchCheck(int batteryKind, cv::Mat image);
};
#endif
