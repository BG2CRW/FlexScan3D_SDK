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
	cv::Mat silkMask(cv::Mat inputImage,cv::Mat edgeMask, cv::Mat adpROI);
	void edgeCut(cv::Mat inputImage);
	cv::Mat edgeMake(cv::Mat origin);
	
	void showDefect(cv::Mat finalShow, cv::Mat inputImage);

	string edgeDetect(cv::Mat inputImage, cv::Mat edgeMask);
	string blackDetect(cv::Mat inputImage, cv::Mat edgeMask);
	string alDetect(cv::Mat origin, cv::Mat inputImage, vector<vector<Point>>& contoursAl);
	string scratchDetect(cv::Mat origin, cv::Mat inputImage);
	string liquidDetect(cv::Mat origin, cv::Mat inputImage, vector<vector<Point>>& contoursLiquid);
	string dirtyDetect(cv::Mat adpModel, cv::Mat imageScratch, cv::Mat imageDirty);
public:
	//Confirm the surface in function "silkMask"
	int surfaceIndex;//1 for English face, 2 for Chinese face

	int batteryKind;
	string scratchCheck(cv::Mat image, cv::Mat& silkModel2d, vector<vector<Point>>& contoursAl, vector<vector<Point>>& contoursLiquid);
};
#endif
