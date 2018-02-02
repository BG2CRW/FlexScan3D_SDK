//Scratch detection (No Chinese)
#ifndef DETECT2D_HPP
#define DETECT2D_HPP

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

class detect2d
{
private:
	int minRow, maxRow, minCol, maxCol;
	
	cv::Mat preProcess(cv::Mat inputImage);
	cv::Mat silkMask(cv::Mat inputImage, cv::Mat adpROI);
	void edgeCut(cv::Mat inputImage);
	
	void showDefect(cv::Mat finalShow, cv::Mat inputImage);

	int alDetect(cv::Mat origin, cv::Mat inputImage);
	int scratchDetect(cv::Mat origin, cv::Mat inputImage);
	int blackDetect(cv::Mat inputImage);
	int liquidDetect(cv::Mat origin, cv::Mat inputImage);

public:
	//Confirm the surface in function "silkMask"
	int surfaceIndex;//1 for English face, 2 for Chinese face

	int batteryKind;
	int scratchCheck(cv::Mat image);
};
#endif
