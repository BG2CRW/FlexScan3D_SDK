#ifndef SHADOW_HPP
#define SHADOW_HPP
#include "opencv2\opencv.hpp"
#include "config.hpp"
#include <pcl\point_cloud.h>
#include <pcl\point_types.h>
#include <pcl\features/normal_3d.h>
#include <pcl\io\pcd_io.h>   
#include <pcl\visualization\cloud_viewer.h> 
#include <opencv2\core/core.hpp>
#include <opencv2\highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>

#include <opencv2\core\utility.hpp>
#include <opencv2\highgui.hpp>
#include <opencv2\opencv.hpp>
#include <cstring>

#include <math.h>
#include <stdlib.h> // For rand() and RAND_MAX
#include <stdio.h>
#include <time.h>
#include <iostream>
using namespace cv;
using namespace std;

class shadow
{
private:
	void cvFitPlane(const CvMat* points, float* plane);
	pcl::PointCloud<pcl::PointXYZ> cutPlanez(pcl::PointCloud<pcl::PointXYZ> CLOUD,float cut_offz,long int *cutNum);
	pcl::PointCloud<pcl::PointXYZ> cutPlanexy(pcl::PointCloud<pcl::PointXYZ> CLOUD,float cutHead,float cutFoot,float cutY,float maxx,float maxy,long int *cutNum);
	pcl::PointCloud<pcl::PointXYZ> rotatePlane(float alpha,float beta,pcl::PointCloud<pcl::PointXYZ> CLOUD,int size,float *Angle);
	void findMin(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *min);
	void findMax(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *max);
	pcl::PointCloud<pcl::PointXYZ>PlaneOffset(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *min);
	cv::Mat shadow3Dto2D(int width,int height,int ratio,pcl::PointCloud<pcl::PointXYZ> CLOUD);
	float computeMean(cv::Mat &image);
	void fitcolor(cv::Mat &image, float delta);
	void cloud2Mat(CvMat* points_mat, pcl::PointCloud<pcl::PointXYZ> cloud);

public:
	pcl::PointCloud<pcl::PointXYZ> resultCloud;
	cv::Mat shadow::showDepthImage(pcl::PointCloud<pcl::PointXYZ> cloud, float cutHead, float cutFoot);
};
#endif //SHADOW_H
