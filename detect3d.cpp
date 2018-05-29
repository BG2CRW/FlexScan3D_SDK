//预处理：滤波+canny
#include "flatulence.hpp"
#include "detect3d.hpp"
#define DEBUG 1
#define PI 3.1415926  

class flatulence Flatulence;

using namespace std;
using namespace cv;

void detect3d::findModel(cv::Mat depthImage,string path,Point* matchLocation,float threshold)
{
	Mat g_srcImage,g_templateImage,g_resultImage;
	g_templateImage = imread(path, 0);  
	depthImage.copyTo(g_srcImage); 
 
    //a) init a matrix to output result
    int resultImage_cols =  g_srcImage.cols - g_templateImage.cols + 1;  
    int resultImage_rows = g_srcImage.rows - g_templateImage.rows + 1;  
    g_resultImage.create( resultImage_cols, resultImage_rows, CV_32FC1 );  
  
    //b) to match
	int g_nMatchMethod=cv::TM_CCOEFF_NORMED;//TM_SQDIFF,TM_SQDIFF_NORMED,TM_CCORR,TM_CCORR_NORMED,TM_CCOEFF,TM_CCOEFF_NORMED
    matchTemplate( g_srcImage, g_templateImage, g_resultImage, g_nMatchMethod );
  
    //c) to locate the best location
    double minValue; double maxValue; Point minLocation; Point maxLocation;   
	
    minMaxLoc( g_resultImage, &minValue, &maxValue, &minLocation, &maxLocation, Mat() );  
    //d) for SQDIFF and SQDIFF_NORMED, the smaller the better, the other is inverse  
	if( g_nMatchMethod  == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED)  
	{ 
		matchLocation[0] = minLocation; 
		//cout<<"min: "<<minValue<<endl;
	}  
	else  
	{ 
		matchLocation[0] = maxLocation; 
		//cout<<"max: "<<maxValue<<endl;
	}  
	matchLocation[1].x=matchLocation[0].x + g_templateImage.cols;
	matchLocation[1].y=matchLocation[0].y + g_templateImage.rows;

	//e) draw a rectangle to show result
	if( g_nMatchMethod  == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED)  
	{
		if(minValue<threshold)	
			rectangle( g_srcImage, matchLocation[0],matchLocation[1],Scalar(0,0,255), 2, 8, 0 ); 
		else
		{
			matchLocation[0].x=0;matchLocation[1].x=0;
			matchLocation[0].y=0;matchLocation[1].y=0;
			
		} 
	}
	if( g_nMatchMethod  == CV_TM_CCORR || g_nMatchMethod == CV_TM_CCORR_NORMED|| \
		 g_nMatchMethod == CV_TM_CCOEFF|| g_nMatchMethod == CV_TM_CCOEFF_NORMED) 
	{   
		if(maxValue>threshold)
			rectangle( g_srcImage, matchLocation[0],matchLocation[1],Scalar(0,0,255), 2, 8, 0 );  
		else
		{
			matchLocation[0].x=0;matchLocation[1].x=0;
			matchLocation[0].y=0;matchLocation[1].y=0;
		}
	}
    //imshow( "result", g_srcImage );   
}

void detect3d::makeMask(cv::Mat depthImage,cv::Mat erodeBinary, cv::Mat silk2D,int threshold,int erodeTimes)
{
	cv::Mat src;
	cv::Mat image2D, image3D;
	double scale, tempx, tempy, x2D,  y2D, x3D, y3D;
	scale = 1.35;
	Point matchLocation_2Drecycle[2];
	Point matchLocation_2Dapple[2];
	Point matchLocation_2Derror[2];
	Point matchLocation_3Drecycle[2];
	Point matchLocation_3Dapple[2];
	Point matchLocation_3Derror[2];
	string path_3Drecycle = "D:/model/model_recycle.png";
	string path_3Derror = "D:/model/model_error.png";
	string path_3Dapple = "D:/model/model_apple.png";
	string path_2Drecycle = "D:/model/model_2Drecycle.png";
	string path_2Derror = "D:/model/model_2Derror.png";
	string path_2Dapple = "D:/model/model_2Dapple.png";
	depthImage.copyTo(src);

	for (int i = 0; i<src.rows; i++)
	{
		for (int j = 0; j<src.cols; j++)
		{
			if (src.at<uchar>(i, j)<threshold)
				src.at<uchar>(i, j) = 0;
			else
				src.at<uchar>(i, j) = 255;
		}
	}
	//imshow("erode",erodeBinary);
	silk2D.copyTo(image2D);
	depthImage.copyTo(image3D);
	cv::imshow("1", image3D);
	waitKey();
	findModel(image2D, path_2Dapple, matchLocation_2Dapple, 0.4);
	findModel(image3D, path_3Dapple, matchLocation_3Dapple, 0.4);
	resize(image2D, image2D, Size(image2D.cols / scale, image2D.rows / scale), 0, 0, INTER_LINEAR);
	matchLocation_2Dapple[0].x /= scale;
	matchLocation_2Dapple[0].y /= scale;
	matchLocation_2Dapple[1].x /= scale;
	matchLocation_2Dapple[1].y /= scale;
    x2D = matchLocation_2Dapple[0].x;
	y2D = matchLocation_2Dapple[0].y;
	rectangle(image2D, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(255, 255, 255), 2, 8, 0);
	rectangle(image3D, matchLocation_3Dapple[0], matchLocation_3Dapple[1], Scalar(0, 0, 255), 2, 8, 0);
	/*rectangle(erodeBinary, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(0, 0, 255), 2, 8, 0);*/
	/*image2D = 255 - image2D;
	rectangle(image2D, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(0, 0, 255), 2, 8, 0);
	rectangle(image3D, matchLocation_3Dapple[0], matchLocation_3Dapple[1], Scalar(0, 0, 255), 2, 8, 0);
	cv::imshow("image2D", image2D);*/
	/*cv::imshow("image3D", erodeBinary);*/
	x3D = matchLocation_3Dapple[0].x;
	y3D = matchLocation_3Dapple[0].y;
	//image2D = 255 - image2D;
	cv::imshow("image2D2", image2D);
	cv::imshow("image3D2", image3D);
	waitKey();
	for (int i = 0; i<src.rows; i++)
	{
		for (int j = 0; j<src.cols; j++)
		{
			tempx = i - y3D + y2D;
			tempy = j - x3D + x2D;
			if (image2D.at<uchar>(tempx, tempy) > 50)
				src.at<uchar>(i, j) = 0;
		}
	}
	Mat element = getStructuringElement(0, Size(2 * erodeTimes + 1, 2 * erodeTimes + 1), Point(erodeTimes, erodeTimes));
	erode(src, erodeBinary, element);
	/*cv::imshow("erodeBinary", erodeBinary);
	waitKey();*/
	/*cout << "matchLocation_2Dapple[0]: " <<matchLocation_2Dapple[0].x << endl;*/
	/*findModel(image2D, path_2Drecycle, matchLocation_recycle, 0.3);
	findModel(image2D, path_2Derror, matchLocation_error, 0.3);*/
}

void ConnectEdge(Mat src)
{
	int width = src.cols;
	int height = src.rows;
	for (int i = 2; i < height - 2; i++)
	{
		for (int j = 2; j < width - 2; j++)
		{
			//如果该中心点为255,则考虑它的八邻域  
			if (src.at<uchar>(i, j) == 255)
			{
				int num = 0;
				for (int k = -1; k < 2; k++)
				{
					for (int l = -1; l < 2; l++)
					{
						//如果八邻域中有灰度值为0的点，则去找该点的十六邻域  
						if (k != 0 && l != 0 && src.at<uchar>(i + k, j + l) == 255)
							num++;
					}
				}
				//如果八邻域中只有一个点是255，说明该中心点为端点，则考虑他的十六邻域  
				if (num == 1)
				{
					for (int k = -2; k < 3; k++)
					{
						for (int l = -2; l < 3; l++)
						{
							//如果该点的十六邻域中有255的点，则该点与中心点之间的点置为255  
							if (!(k < 2 && k > -2 && l < 2 && l > -2) && src.at<uchar>(i + k, j + l) == 255)
							{
								src.at<uchar>(i + k / 2, j + l / 2) = 255;
							}
						}
					}
				}
			}
		}
	}
}

int detect3d::check3d(cv::Mat depthImage,cv::Mat silk2D)
{

	int id = 0;
	int classify[1];
	int inv[1];
	int pos[1];
	Point matchLocation[2];
	Mat pic, pic_inv;
	int type=0;//0:big battery,1:small battery

	depthImage.copyTo(pic);
	Mat filterImage, canny, erodeImg, maskErode, blackMask,grad_x,grad_y,abs_grad_x,abs_grad_y,dst,lapalace,abs_lapalace,canny1;
	depthImage.copyTo(blackMask);
	makeMask(depthImage, blackMask, silk2D, 5, 5);
	imshow("blackMask",blackMask);

	Sobel(depthImage, grad_x, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	imshow("x direction Sobel", abs_grad_x);

	Sobel(depthImage, grad_y, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	imshow("y direction Sobel", abs_grad_y);

	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
	imshow("all direction Sobel", dst);

	Canny(depthImage, canny1, 10, 110, 3);
	//imshow("canny1",canny1);
	dst.copyTo(canny);

	for(int i = 0; i<canny.rows; i++)
	{
		for (int j = 0; j<canny.cols; j++)
		{
			if (canny.at<uchar>(i, j) > 25)
				canny.at<uchar>(i, j) = 255;
			else
				canny.at<uchar>(i, j) = 0;
		}
	}
	imshow("dst", canny);

/*
	for (int i = 0; i<canny.rows; i++)
	{
		for (int j = 0; j<canny.cols; j++)
		{
			canny.at<uchar>(i, j) *= (blackMask.at<uchar>(i, j) / 255);
		}
	}
*/	
	//imshow("filter",filterImage);
	imshow("canny",canny);

	Mat element = getStructuringElement(MORPH_RECT, Size(1, 1), Point(-1, -1));
	Mat Mask;
	erode(255 - canny, Mask, element);
	imshow("Mask", Mask);
	//waitKey();
	ConnectEdge(Mask);
	imshow("Mask1", Mask);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(Mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
	Mat drawing = Mat::zeros(Mask.size(), CV_8U);
	int j = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
								   //如果面积超出了设定的范围，则不再考虑该斑点  

		if (area > 27 && area < 10000)
		{
			drawContours(drawing, contours, i, Scalar(255), FILLED, 8, hierarchy, 0, Point());
			j = j + 1;

		}
	}

	for (int i = 0; i<drawing.rows; i++)
	{
		for (int j = 0; j<drawing.cols; j++)
		{
			drawing.at<uchar>(i, j) *= (blackMask.at<uchar>(i, j) / 255);
		}
	}
	cv::Mat element15(3, 3, CV_8U, cv::Scalar(1));
	cv::Mat close;
	cv::morphologyEx(drawing, close, cv::MORPH_CLOSE, element15);
	imshow("drawing", drawing);
	//waitKey();
	vector<vector<Point> > contours1;
	vector<Vec4i> hierarchy1;
	findContours(close, contours1, hierarchy1, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//imshow("close", close);
	j = 0;
	int m = 0;
	for (int i = 0; i < contours1.size(); i++)
	{
		Moments moms = moments(Mat(contours1[i]));
		double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
								   //如果面积超出了设定的范围，则不再考虑该斑点  

		double area1 = contourArea(contours1[i]);
		if (area >27 && area < 100000)
		{
			drawContours(depthImage, contours1, i, Scalar(0, 0, 255), FILLED, 8, hierarchy1, 0, Point());
			j = j + 1;

		}
		else if (area >= 0 && area <= 27)
		{
			drawContours(depthImage, contours1, i, Scalar(255, 0, 0), FILLED, 8, hierarchy1, 0, Point());
			m = m + 1;
		}
	}

	char t[256];
	cout << j << endl;

	imshow("漏洞", depthImage);
	waitKey();
	drawing.copyTo(depthImage);
	

	return j;
}

cv::Mat detect3d::drawResult(cv::Mat src,cv::Mat imgFCN)
{
	//Find the damages
	vector<vector<Point>> contours;
	vector<vector<Point>> contoursvalue; 
	vector<Vec4i> hierarchy;
	Mat image1 = imgFCN.clone();
	Mat newsrc = src.clone();
	findContours(image1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	vector<double> length;
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;
		double templength = arcLength(contours[i], true);    //compute the perimeter

		if (area > 40 )	//choose right area
		{
			//cout << "are: " << area << endl;
			 
				contoursvalue.push_back(contours[i]);
				length.push_back(templength);

				//cout << "The area: " << area << " and templength: " << templength << endl;
			}
	}
	for (int i = 0; i < contoursvalue.size(); i++)
	{
		drawContours(newsrc, contoursvalue, i, Scalar(0), 1, 8, hierarchy, 0, Point());
	}
	//cv::imshow("result", newsrc);
	return newsrc;

}

int detect3d::errorReport(cv::Mat imgdepthVert, cv::Mat imgdepthHor, cv::Mat silk2D)
{
	int report;
	report = Flatulence.flatulenceCheck(imgdepthVert, 127, 2.3, 240, 35);
	report = Flatulence.flatulenceCheck(imgdepthHor, 127, 2.3, 240, 35);
	//check3d(imgdepthVert, silk2D);
	check3d(imgdepthHor, silk2D);
	return report;
}

