#include "stdafx.h"
#include "Callbacks.h"
#include "shadow.hpp"
#include <time.h>
#include <boost/timer.hpp>
#include "config.hpp"
#define PI 3.1415926
using namespace cv;
class shadow Shadow;


void ScanProcessedCallback(void* userContext, FS3D_Handle handle)
{
	// Get the scan
	int nVertices;
	int numValues;
	double *vertices;
	int errorReport=0;
	boost::timer constant;
	if (FS3D_GetInt(handle, "nVertices", &nVertices) != FS3D_RESULT_OK)
	{
		printf("Could not retrieve the number of vertices.\n");
		return;
	}
   
	if (FS3D_GetDoubleArray(handle, "vertices", &numValues, &vertices) != FS3D_RESULT_OK)
	{
		printf("Could not retrieve vertices.\n");
		return;
	}

	printf("Scan acquired! Num vertices: %d\n", nVertices);

	// example of looping through all vertices
	// Users should copy the data if we need retain the data. Data buffer passed to the callbacks are freed when the callback handlers come back.  
	pcl::PointCloud<pcl::PointXYZ> cloud;
	cloud.width = nVertices;
	cloud.height = 1;
	cloud.points.resize(cloud.width * cloud.height);
	for (int i = 0; i < nVertices; i++)
	{
		cloud.points[i].x = (float)vertices[i * 3];
		cloud.points[i].y = (float)vertices[i * 3 + 1];
		cloud.points[i].z = -(float)vertices[i * 3 + 2];
	}
	cout << "Total time: " << constant.elapsed() << endl;
	boost::timer t1;
	cv::Mat depthSrc;
	depthSrc = Shadow.showDepthImage(cloud, 2, 13);
	if (depthSrc.rows == 3)
	{
		return;
	}
	
	/*errorReport = Flatulence.flatulenceCheck(depthSrc, 127, 2.3, 240, 35);*/
	if (depthSrc.cols > depthSrc.rows)
	{
		transpose(depthSrc, depthSrc);
		flip(depthSrc, depthSrc, 1);
	}
	int target_width = 630;
	int target_height = 1200;

	cv::Mat padding = Mat::zeros(target_height, target_width, CV_8UC1);
	for (int i = 0; i<depthSrc.rows; i++)
	{
		for (int j = 0; j<depthSrc.cols; j++)
		{
			padding.at<uchar>(i, j) = depthSrc.at<uchar>(i, j);
		}
	}

	cv::imwrite("cache.jpg", padding);
	cout << "Time of flatulence check: " << t1.elapsed() << endl;
	if (errorReport == 1 || errorReport == 2)
	{
		return;
	}

}
