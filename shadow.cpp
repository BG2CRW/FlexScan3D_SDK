#include "stdafx.h"
#include "shadow.hpp"

#define BLACK_THRESHOLD 0.02
#define PI 3.1415926

using namespace std;
using namespace cv;



void shadow::cloud2Mat(CvMat* points_mat, pcl::PointCloud<pcl::PointXYZ> cloud)
{
	for (int i = 0; i < cloud.width; i++)
	{
		points_mat->data.fl[i * 3 + 0] = cloud.points[i].x;
		points_mat->data.fl[i * 3 + 1] = cloud.points[i].y;
		points_mat->data.fl[i * 3 + 2] = cloud.points[i].z;
	}
}

//Ax+by+cz=D  
void shadow::cvFitPlane(const CvMat* points, float* plane)
{  
	// Estimate geometric centroid.  
	int nrows = points->rows;  
	int ncols = points->cols;  
	int type = points->type;  
	CvMat* centroid = cvCreateMat(1, ncols, type);  
	cvSet(centroid, cvScalar(0));  
	for (int c = 0; c<ncols; c++)
	{  
	    for (int r = 0; r < nrows; r++)  
	    {  
		centroid->data.fl[c] += points->data.fl[ncols*r + c];  
	    }  
	    centroid->data.fl[c] /= nrows;  
	}  
	// Subtract geometric centroid from each point.  
	CvMat* points2 = cvCreateMat(nrows, ncols, type);  
	for (int r = 0; r<nrows; r++)  
	    for (int c = 0; c<ncols; c++)  
		points2->data.fl[ncols*r + c] = points->data.fl[ncols*r + c] - centroid->data.fl[c];  
	// Evaluate SVD of covariance matrix.  
	CvMat* fitA = cvCreateMat(ncols, ncols, type);  
	CvMat* fitW = cvCreateMat(ncols, ncols, type);
	CvMat* fitV = cvCreateMat(ncols, ncols, type);
	cvGEMM(points2, points, 1, NULL, 0, fitA, CV_GEMM_A_T);
	cvSVD(fitA, fitW, NULL, fitV, CV_SVD_V_T);
	// Assign plane coefficients by singular vector corresponding to smallest singular value.  
	plane[ncols] = 0;  
	for (int c = 0; c<ncols; c++)
	{  
	    plane[c] = fitV->data.fl[ncols*(ncols - 1) + c];
	    plane[ncols] += plane[c] * centroid->data.fl[c];  
	}  
	// Release allocated resources.   
	cvReleaseMat(&points2);  
	cvReleaseMat(&fitA);
	cvReleaseMat(&fitW);
	cvReleaseMat(&fitV);
}  

pcl::PointCloud<pcl::PointXYZ> shadow::rotatePlane(float alpha,float beta,pcl::PointCloud<pcl::PointXYZ> CLOUD,int size,float *Angle)
{
	Mat T0=Mat::ones(3,3,CV_32FC1);  
	Mat T1=Mat::ones(3,3,CV_32FC1); 
	Mat T;
	Mat old_position=Mat::ones(3,1,CV_32FC1);
	Mat new_position=Mat::ones(3,1,CV_32FC1);

	T0.at<float>(0,0)=1;  
	T0.at<float>(0,1)=0;
	T0.at<float>(0,2)=0;  
	T0.at<float>(1,0)=0;  
	T0.at<float>(1,1)=cos(alpha); 
	T0.at<float>(1,2)=-sin(alpha); 
	T0.at<float>(2,0)=0;  
	T0.at<float>(2,1)=sin(alpha);
	T0.at<float>(2,2)=cos(alpha);

	T1.at<float>(0,0)=cos(beta);  
	T1.at<float>(0,1)=0;  
	T1.at<float>(0,2)=sin(beta);  
	T1.at<float>(1,0)=0;  
	T1.at<float>(1,1)=1;  
	T1.at<float>(1,2)=0; 
	T1.at<float>(2,0)=-sin(beta);  
	T1.at<float>(2,1)=0;  
	T1.at<float>(2,2)=cos(beta); 

	T=T1*T0;

	CLOUD.width  = size;
	CLOUD.height = 1;
	CLOUD.points.resize(CLOUD.width * CLOUD.height);

	for(int i=0;i<size;i++)
	{
		old_position.at<float>(0,0)=CLOUD.points[i].x; 
		old_position.at<float>(1,0)=CLOUD.points[i].y; 
		old_position.at<float>(2,0)=CLOUD.points[i].z; 

		new_position=T*old_position;  

		CLOUD.points[i].x=new_position.at<float>(0,0);
		CLOUD.points[i].y=new_position.at<float>(1,0);
		CLOUD.points[i].z=new_position.at<float>(2,0);

	}
	return CLOUD;
}

pcl::PointCloud<pcl::PointXYZ> shadow::cutPlanez(pcl::PointCloud<pcl::PointXYZ> CLOUD,float cut_offz,long int *cutNum)
{
	for(int i=0;i<CLOUD.size();i++)
	{
		if(CLOUD.points[i].z<cut_offz)
		{
			CLOUD.points[i].z=-1001;
			CLOUD.points[i].y=-1001;
			CLOUD.points[i].x=-1001;
			*cutNum=*cutNum+1;
		}
	}
	return CLOUD;
}

pcl::PointCloud<pcl::PointXYZ> shadow::cutPlanexy(pcl::PointCloud<pcl::PointXYZ> CLOUD,float cutHead,float cutFoot,float cutY,float maxx,float maxy,long int *cutNum)
{
	for(int i=0;i<CLOUD.size();i++)
	{
		if(CLOUD.points[i].x<cutHead || CLOUD.points[i].x>maxx-cutFoot || CLOUD.points[i].y<cutY || CLOUD.points[i].y>maxy-cutY)
		{
			CLOUD.points[i].z=-1001;
			CLOUD.points[i].y=-1001;
			CLOUD.points[i].x=-1001;
			*cutNum = *cutNum + 1;
		}
	}
	return CLOUD;
}

void shadow::findMin(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *min)
{
	min[0]=1000;min[1]=1000;min[2]=1000;
	for(int i=0;i<CLOUD.size();i++)
	{
		if((CLOUD.points[i].x<min[0])&&(CLOUD.points[i].x!=-1001))
		{
			min[0]=CLOUD.points[i].x;
		}
		if((CLOUD.points[i].y<min[1])&&(CLOUD.points[i].y!=-1001))
		{
			min[1]=CLOUD.points[i].y;
		}
		if((CLOUD.points[i].z<min[2])&&(CLOUD.points[i].z!=-1001))
		{
			min[2]=CLOUD.points[i].z;
		}
	}
}

void shadow::findMax(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *max)
{
	max[0]=-1000;max[1]=-1000;max[2]=-1000;
	for(int i=0;i<CLOUD.size();i++)
	{
		if((CLOUD.points[i].x>max[0])&&(CLOUD.points[i].x!=-1001))
		{
			max[0]=CLOUD.points[i].x;
		}
		if((CLOUD.points[i].y>max[1])&&(CLOUD.points[i].y!=-1001))
		{
			max[1]=CLOUD.points[i].y;
		}
		if((CLOUD.points[i].z>max[2])&&(CLOUD.points[i].z!=-1001))
		{
			max[2]=CLOUD.points[i].z;
		}
	}
}

pcl::PointCloud<pcl::PointXYZ> shadow::PlaneOffset(pcl::PointCloud<pcl::PointXYZ> CLOUD,float *min)
{
	for(int i=0;i<CLOUD.size();i++)
	{
		if((CLOUD.points[i].x!=-1001)&&(CLOUD.points[i].y!=-1001)&&(CLOUD.points[i].z!=-1001))
		{
			CLOUD.points[i].x-=(min[0]);
			CLOUD.points[i].y-=(min[1]);
			CLOUD.points[i].z-=(min[2]);
		}
	}
	return CLOUD;
}

float shadow::computeMean(cv::Mat &image)
{
	int nr = image.rows; // number of rows  
	int nc = image.cols;
	int channel = image.channels(); // total number of elements per line  
	float mean = 0;
	int cnt_black_point = 0;
	for (int j = 0; j<nr; j++)
	{
		float* data = image.ptr<float>(j);
		for (int i = 0; i<nc; i++)
		{
			if (data[i] <= BLACK_THRESHOLD)
			{
				cnt_black_point++;
				data[i] = 0;
			}
		}
	}

	int total = nr*nc - cnt_black_point;

	for (int j = 0; j<nr; j++)
	{
		float* data = image.ptr<float>(j);
		for (int i = 0; i<nc; i++)
		{
			if (data[i]>BLACK_THRESHOLD)
				mean += (float)data[i] / (float)total;
		}
	}
	return mean;
}

void shadow::fitcolor(cv::Mat &image, float delta)
{
	float mean = 0;
	for (int j = 0; j<image.rows; j++)
	{
		float* data = image.ptr<float>(j);
		for (int i = 0; i<image.cols; i++)
		{
			if (data[i]>BLACK_THRESHOLD)
			{
				data[i] += delta;
			}
		}
	}
}

cv::Mat shadow::shadow3Dto2D(int width,int height,int ratio,pcl::PointCloud<pcl::PointXYZ> CLOUD)
{
#if DEBUG
	cout<<width<<"and"<<height<<endl;
#endif
	cv::Mat weight;
	cv::Mat depthImage;
	weight=Mat::zeros(width*ratio+1,height*ratio+1,CV_32FC1); //save weights
	depthImage=cv::Mat::zeros(width*ratio+1,height*ratio+1,CV_32FC1);
	int invalid_num=0;
	int valid_num=0;
	int hole=0;
	int row=0;
	int col=0;
	int rowplus=0;
	int rowminus=0;
	int colplus=0;
	int colminus=0;
	float max=-10;
	for(int i=0;i<CLOUD.size();i++)
	{
		if((CLOUD.points[i].x!=-1001)&&(CLOUD.points[i].y!=-1001)&&(CLOUD.points[i].z!=-1001))
		{
			if(CLOUD.points[i].x>width) CLOUD.points[i].x=width;//avoid stack overflow
			if(CLOUD.points[i].y>height) CLOUD.points[i].y=height;
			//CLOUD.points[i].x*ratio
			row=(int)(CLOUD.points[i].x*ratio);
			col=(int)(CLOUD.points[i].y*ratio);
			rowplus=row+1;
			rowminus=row-1;
			colplus=col+1;
			colminus=col-1;

			if(row==0) rowminus=0;
			if(row==width*ratio) rowplus=row;
			if(col==0) colminus=0;
			if(col==height*ratio) colplus=col;

			depthImage.at<float>(row,col)+=CLOUD.points[i].z;
			depthImage.at<float>(rowplus,colplus)+=CLOUD.points[i].z;
			depthImage.at<float>(rowplus,colminus)+=CLOUD.points[i].z;
			depthImage.at<float>(rowminus,colminus)+=CLOUD.points[i].z;
			depthImage.at<float>(rowminus,colplus)+=CLOUD.points[i].z;

			weight.at<float>(row,col)++;
			weight.at<float>(rowplus,colplus)++;
			weight.at<float>(rowplus,colminus)++;
			weight.at<float>(rowminus,colplus)++;
			weight.at<float>(rowminus,colminus)++;

			valid_num++;
		}
		else
			invalid_num++;
	}

	for(int j=0;j<width*ratio+1;j++)
	{
		for(int k=0;k<height*ratio+1;k++)
		{
			if(weight.at<float>(j,k)==0)
			{
				weight.at<float>(j,k)=1;
				depthImage.at<float>(j,k)=0;
				hole++;
			}
			
			depthImage.at<float>(j,k)=depthImage.at<float>(j,k)/weight.at<float>(j,k);

			if(depthImage.at<float>(j,k)>max)
				max=depthImage.at<float>(j,k);
		}	
	}

	//for(int j=0;j<width*ratio+1;j++)
	//{
	//	for(int k=0;k<height*ratio+1;k++)
	//	{
	//		depthImage.at<float>(j,k)=depthImage.at<float>(j,k)/1;//max;
	//	}	
	//}
#if DEBUG
	//cout<<valid_num<<","<<invalid_num<<","<<hole<<","<<max<<endl;
#endif
	return depthImage;
}

cv::Mat shadow::showDepthImage(pcl::PointCloud<pcl::PointXYZ> cloud, float cutHead, float cutFoot)
{
	cv::Mat temp;
	float Min[3] = { 0 };
	float Max[3] = { 0 };
	float angle[2] = { 0 };
	float plane_vector[4] = { 0 };
	float A, B, C, D;
	int valid = 0;
	long int num = 0;
	long int cutNum = 0;

	pcl::PointCloud<pcl::PointXYZ> cloud3;
	CvMat*points_mat = cvCreateMat(cloud.points.size(), 3, CV_32FC1);//define a matrix to save points need fit
	cloud2Mat(points_mat, cloud);

//#if DEBUG
//	//fit a plane
//	cvFitPlane(points_mat, plane_vector);//define an array to save the plane parameters
//	A=plane_vector[0];
//	B=plane_vector[1];
//	C=plane_vector[2];
//	D=plane_vector[3];
//	
//	cout<<"Plane: "<< plane_vector[0]<<" "<< plane_vector[1]<<" "<< plane_vector[2]<<" "<< plane_vector[3]<<"\n";
//	cloud=rotatePlane(atan(B/C),-atan(A/C),cloud, cloud.points.size(),angle);
//#endif
//
//	findMin(cloud, Min);
	findMax(cloud, Max);
	float k = 2.1;
	cloud = cutPlanez(cloud, Max[2] - k, &cutNum);
	if (cloud.points.size() < 10000)
	{
		cout << "Error: No object!" << endl;
		temp = Mat::zeros(3, 3, CV_8UC1);
		return temp;
	}

	findMin(cloud,Min);
	cloud=PlaneOffset(cloud,Min);
	findMin(cloud,Min);
	findMax(cloud,Max);
#if DEBUG
	cout<<"min="<<Min[0]<<","<<Min[1]<<","<<Min[2]<<endl;
	cout<<"max="<<Max[0]<<","<<Max[1]<<","<<Max[2]<<endl;
#endif

	//select the valid points and send into cloud3
	valid = cloud.points.size() - cutNum;
	cloud3.width  = valid;
	cloud3.height = 1;
	cloud3.points.resize(cloud3.width * cloud3.height);
	valid = 0;
	for(int m=0;m<cloud.size();m++)
	{
		if ((cloud.points[m].x != -1001) && (cloud.points[m].y != -1001) && (cloud.points[m].z != -1001))
		{
			cloud3.points[valid].x = cloud.points[m].x;
			cloud3.points[valid].y = cloud.points[m].y;
			cloud3.points[valid].z = cloud.points[m].z;
			valid++;
		}
	}
	
	//fit cloud3 and rotate it
	CvMat*points_mat3 = cvCreateMat(cloud3.points.size(), 3, CV_32FC1);//define an array to save the plane parameters
	cloud2Mat(points_mat3, cloud3);
	angle[2] = { 0 };
	plane_vector[4] = { 0 };
	cvFitPlane(points_mat3, plane_vector);
	A = plane_vector[0];
	B = plane_vector[1];
	C = plane_vector[2];
	D = plane_vector[3];
#if DEBUG
	cout << "New Plane: " << plane_vector[0] << " " << plane_vector[1] << " " << plane_vector[2] << " " << plane_vector[3] << "\n";
#endif
	cloud3 = rotatePlane(atan(B / C), -atan(A/C), cloud3, valid, angle);
	
	findMin(cloud3,Min);
	cloud3=PlaneOffset(cloud3,Min);

#if DEBUG
	findMin(cloud3, Min);
	findMax(cloud3, Max);
	cout<<"Final min="<<Min[0]<<","<<Min[1]<<","<<Min[2]<<endl;
	cout<<"Final max="<<Max[0]<<","<<Max[1]<<","<<Max[2]<<endl;
	//Show the pcl cloud
	//pcl::visualization::CloudViewer viewer("pcd viewer");
	//pcl::PointCloud<pcl::PointXYZ>::Ptr cloudPointer(new pcl::PointCloud<pcl::PointXYZ>);
	//cloudPointer = cloud3.makeShared();
	//viewer.showCloud(cloudPointer);
	//int pause666=scanf("%d",&pause666);
#endif
	
	cv::Mat src=shadow3Dto2D(Max[0],Max[1],10,cloud3);
	float mean = computeMean(src);
	cout << "Mean: " << mean << endl;
	fitcolor(src, 0.5 - mean);

	src.convertTo(temp, CV_8U,255.0);
	return temp;
}