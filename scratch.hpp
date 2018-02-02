//Scratch detection (No Chinese)
#ifndef SCRATCH_HPP
#define SCRATCH_HPP

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

class scratch
{
private:
	cv::Mat silkMask(cv::Mat inputImage);
public:
	int scratchCheck(cv::Mat image);
};
#endif
