
// This program publishes randomly?generated velocity
// messages for turtlesim .

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include <cstring>

#include<math.h>
#include <stdlib.h> // For rand() and RAND_MAX
#include <stdio.h>
#include <time.h>
#include <string.h>

#include <shadow.hpp>
#include <flatulence.hpp>
#include <scratch.hpp>
#include <aotupoint.hpp>
#include <aotumark.hpp>
#include <prehandle.hpp>

#define PI 3.1415926

using namespace std;
using namespace cv;

class shadow Shadow;
class flatulence Flatulence;
class scratch Scratch;
class aotupoint Aotupoint;
class aotumark Aotumark;
class prehandle Prehandle;

int error_id=0;

int main (int argc, char **argv)
{
	if(argc!=2)
	{
		cout<<"usage:battery_test No.picture"<<endl;
		exit(0);
	}

	pcl::PointCloud<pcl::PointXYZ> cloud;

	char d[50]="";     
	char b[50]="../source/";
	char c[50]=".asc.";
	char e[50]=".asc";
	char path1[50]="";
	strcpy(c,b);
	strcat(c,argv[1]);
	strcpy(path1,c);
	strcat(path1,e);
#if DEBUG		
	cout<<path1<<endl;
#endif
	int index=atoi(argv[1]);

	cv::Mat src,depthSrc;
	double m=0,sd=0;
	depthSrc=Shadow.showDepthImage(path1,2,13,index);
	cloud=Shadow.resultCloud;
#if DEBUG
	cv::imshow("DepthImage",depthSrc);
	cv::waitKey();
#endif
	//初步筛选		
	error_id=Prehandle.prejudge(depthSrc);
	if(error_id!=0)
	{
		cout<<"shape change!"<<endl;
		if(error_id==1)
			cout<<"aotuhen"<<endl;
		if(error_id==2)
			cout<<"aotudian"<<endl;
		if(error_id==3)
			cout<<"qizhang"<<endl;
		return 0;
	}
	if(error_id==0)
		cout<<"prehandle pass!"<<endl;

/*
	//气胀检测
	error_id=Flatulence.flatulenceCheck(depthSrc);
	if(error_id!=0)
	{
		if(error_id==1)
			cout<<"Warnning: This battery has bad flatulence!"<<endl;
		if(error_id==2)
			cout<<"Warnning: This battery has partial flatulences or other defects!"<<endl;
		return 0;
	}
	if(error_id==0)
		cout<<"Flatulence pass!"<<endl;

	//划痕检测
	char dd[50]="";     
	char bb[50]="../source/";
	char cc[50]=".bmp.";
	char ee[50]=".bmp";
	char path2[50]="";
	strcpy(cc,bb);
	strcat(cc,argv[1]);
	strcpy(path2,cc);
	strcat(path2,ee);
#if DEBUG		
	cout<<path2<<endl;
#endif
	src=imread(path2);

	error_id=Scratch.scratchCheck(src);
	if(error_id!=0)
	{
		if(error_id<4)
			cout<<"Warnning: This battery has "<<error_id<<" scratch!"<<endl;
		if(error_id>3)
			cout<<"Error: The image is of poor quality! Scratch check failed."<<endl;
		//return 0;
	}
	if(error_id==0)
		cout<<"Scratch pass!"<<endl;
*/

/*
	//凹凸点检测
	//vector<cv::Point2d> errorAotupoint;
	//errorAotupoint=Aotupoint.aotupointCheck(src);
/*
	//凹凸痕检测
	vector<cv::Point2d> errorAotumark;
	errorAotumark=Aotumark.aotumarkCheck(src);
*/
	//点云显示
#if DEBUG
	pcl::visualization::CloudViewer viewer("pcd viewer");
	pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPointer(new pcl::PointCloud<pcl::PointXYZ>);  
	cloudPointer = cloud.makeShared();  
	viewer.showCloud(cloudPointer);
	//int pause555=scanf("%d",&pause555);
#endif
    return 0;
}

