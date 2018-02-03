//预处理：滤波+canny
#include "detect3d.hpp"
#define DEBUG 1
#define PI 3.1415926  

using namespace std;
using namespace cv;

void detect3d::findModel(cv::Mat depthImage,string path,Point* matchLocation,float threshold)
{
	Mat g_srcImage,g_templateImage,g_resultImage;
	g_templateImage = imread(path, 0 );  
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

void detect3d::makeMask(cv::Mat depthImage,cv::Mat erodeBinary,int threshold,int erodeTimes,int* classify,Point* matchLocation,int type)
{
	cv::Mat src;
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

	//imshow("mask1",src);
	//waitKey();
	if (type == 0)//big apple battery
	{
		//make silk mask
		if (*classify == 0)//recycle
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);
			rectangle(src, Point(matchLocation[0].x + 30, matchLocation[0].y - 660), \
				Point(matchLocation[0].x + 75, matchLocation[0].y - 605), Scalar(0, 0, 255), CV_FILLED, 8, 0);//apple

			rectangle(src, Point(matchLocation[0].x, matchLocation[0].y + 88), \
				Point(matchLocation[0].x + 155, matchLocation[0].y + 155), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line

			rectangle(src, Point(matchLocation[0].x - 10, matchLocation[0].y - 550), \
				Point(matchLocation[0].x + 115, matchLocation[0].y - 500), Scalar(0, 0, 255), CV_FILLED, 8, 0);//words

			rectangle(src, Point(matchLocation[0].x + 20, matchLocation[0].y - 100), \
				Point(matchLocation[0].x + 75, matchLocation[0].y - 40), Scalar(0, 0, 255), CV_FILLED, 8, 0);//logo

			rectangle(src, Point(0, 0), Point(depthImage.cols, 10), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			rectangle(src, Point(0, matchLocation[0].y + 200), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 1)//error
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);

			rectangle(src, Point(matchLocation[0].x - 75, matchLocation[0].y + 275), \
				Point(matchLocation[0].x + 85, matchLocation[0].y + 320), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line

			rectangle(src, Point(matchLocation[1].x, matchLocation[1].y), \
				Point(matchLocation[0].x + 150, matchLocation[0].y), Scalar(0, 0, 255), CV_FILLED, 8, 0);//error right

			rectangle(src, Point(matchLocation[0].x - 5, matchLocation[0].y - 630), \
				Point(matchLocation[0].x + 175, matchLocation[0].y - 530), Scalar(0, 0, 255), CV_FILLED, 8, 0);//tags
			rectangle(src, Point(0, 0), Point(depthImage.cols, 40), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			rectangle(src, Point(0, matchLocation[0].y + 335), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 2)//apple
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);

			rectangle(src, Point(matchLocation[0].x, matchLocation[0].y + 760), \
				Point(matchLocation[0].x + 200, matchLocation[0].y + 800), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line
			rectangle(src, Point(0, 0), Point(depthImage.cols, 15), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			rectangle(src, Point(0, matchLocation[0].y + 860), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 3)//no
		{
			rectangle(src, Point(0, 0), Point(depthImage.cols, 40), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header
			rectangle(src, Point(0, depthImage.rows - 50), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom
		}
	}
	if (type == 1)//small apple battery
	{
		if (*classify == 0)//recycle
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);
			rectangle(src, Point(matchLocation[0].x + 30, matchLocation[0].y - 620), \
				Point(matchLocation[0].x + 75, matchLocation[0].y - 555), Scalar(0, 0, 255), CV_FILLED, 8, 0);//apple

			//rectangle(src, Point(matchLocation[0].x, matchLocation[0].y + 88), \
				Point(matchLocation[0].x + 155, matchLocation[0].y + 155), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line

			//rectangle(src, Point(matchLocation[0].x - 10, matchLocation[0].y - 550), \
				Point(matchLocation[0].x + 115, matchLocation[0].y - 500), Scalar(0, 0, 255), CV_FILLED, 8, 0);//words

			rectangle(src, Point(matchLocation[0].x + 20, matchLocation[0].y - 100), \
				Point(matchLocation[0].x + 75, matchLocation[0].y - 40), Scalar(0, 0, 255), CV_FILLED, 8, 0);//logo

			rectangle(src, Point(0, 0), Point(depthImage.cols, 10), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			//rectangle(src, Point(0, matchLocation[0].y + 200), \
							Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 1)//error
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);

			//rectangle(src, Point(matchLocation[0].x - 75, matchLocation[0].y + 275), \
				Point(matchLocation[0].x + 85, matchLocation[0].y + 320), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line

			rectangle(src, Point(matchLocation[1].x, matchLocation[1].y), \
				Point(matchLocation[0].x + 130, matchLocation[0].y), Scalar(0, 0, 255), CV_FILLED, 8, 0);//error right

			rectangle(src, Point(matchLocation[0].x - 20, matchLocation[0].y-7), \
				Point(matchLocation[0].x - 40, matchLocation[0].y+17), Scalar(0, 0, 255), CV_FILLED, 8, 0);//error left

			rectangle(src, Point(matchLocation[0].x , matchLocation[0].y +200), \
				Point(matchLocation[0].x + 185, matchLocation[0].y +280), Scalar(0, 0, 255), CV_FILLED, 8, 0);//tags
			//rectangle(src, Point(0, 0), Point(depthImage.cols, 40), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			rectangle(src, Point(0, matchLocation[0].y + 300), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 2)//apple
		{
			rectangle(src, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), CV_FILLED, 8, 0);

			rectangle(src, Point(matchLocation[0].x, matchLocation[0].y + 760), \
				Point(matchLocation[0].x + 200, matchLocation[0].y + 800), Scalar(0, 0, 255), CV_FILLED, 8, 0);//line
			rectangle(src, Point(0, 0), Point(depthImage.cols, 15), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header

			rectangle(src, Point(0, matchLocation[0].y + 860), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom

		}
		if (*classify == 3)//no
		{
			rectangle(src, Point(0, 0), Point(depthImage.cols, 30), Scalar(0, 0, 255), CV_FILLED, 8, 0);//header
			rectangle(src, Point(0, depthImage.rows - 40), \
				Point(depthImage.cols, depthImage.rows), Scalar(0, 0, 255), CV_FILLED, 8, 0);//bottom
		}
	}
	//imshow("mask2",src);

	Mat element = getStructuringElement(0, Size(2 * erodeTimes + 1, 2 * erodeTimes + 1), Point(erodeTimes, erodeTimes));
	erode(src, erodeBinary, element);

	//imshow("erode",erodeBinary);
}

//	inv       0:pos,1:neg,2:no
//	pos       0:pos,1:neg,2:no
//	classify  0:recycle,1:error,2:apple,3:no

void detect3d::judgeInv(int *classify,int *inv,int *pos,Mat depthImage,Point* matchLocation)
{
	string path_recycle="/home/chaofan/huahen/model/model_recycle.png";
	string path_error="/home/chaofan/huahen/model/model_error.png";
	string path_apple="/home/chaofan/huahen/model/model_apple.png";

	Mat pic,pic_inv;
	depthImage.copyTo(pic);

	inv[0]=0;//0:pos,1:neg,2:no
	pos[0]=0;//0:pos,1:neg,2:no
	classify[0]=0;//0:recycle,1:error,2:apple,3:no

	findModel(pic,path_recycle,matchLocation,0.4);
	if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
		findModel(pic,path_error,matchLocation,0.25);
		if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
			flip(pic,pic_inv,-1);
			findModel(pic_inv,path_recycle,matchLocation,0.4);
			if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
				findModel(pic_inv,path_error,matchLocation,0.25);
				if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
					findModel(pic,path_apple,matchLocation,0.5);
					if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
						findModel(pic_inv,path_apple,matchLocation,0.5);
						if((matchLocation[0].x==0)&&(matchLocation[0].y==0)){
							inv[0]=2;pos[0]=2;classify[0]=3;
						}
						else{
							inv[0]=1;pos[0]=0;classify[0]=2;
						}
					}
					else{
						inv[0]=0;pos[0]=0;classify[0]=2;
					}
				}
				else{
					inv[0]=1;pos[0]=1;classify[0]=1;
				}
			}
			else{
				inv[0]=1;pos[0]=0;classify[0]=0;
			}
		}
		else{
			inv[0]=0;pos[0]=1;classify[0]=1;
		}
	}
	else{
		inv[0]=0;pos[0]=0;classify[0]=0;
	}
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

int detect3d::prejudge(cv::Mat depthImage)
{

	int id = 0;
	int classify[1];
	int inv[1];
	int pos[1];
	Point matchLocation[2];
	Mat pic, pic_inv;
	int type=0;//0:big battery,1:small battery
	
	depthImage.copyTo(pic);
	flip(pic, pic_inv, -1);

	judgeInv(classify, inv, pos, depthImage, matchLocation);
	cout << classify[0] << " " << inv[0] << " " << pos[0] << " " << matchLocation[0].x << " " << matchLocation[0].y << " " << matchLocation[1].x << " " << matchLocation[1].y << endl;

	if (inv[0] == 0)
	{
		rectangle(pic, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), 2, 8, 0);
		imshow("rect",pic);
	}

	if (inv[0] == 1)
	{
		pic_inv.copyTo(depthImage);
		rectangle(pic_inv, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), 2, 8, 0);
		imshow("rect",pic_inv);
	}
	imshow("src", depthImage);
	cout<<"height:" <<depthImage.rows<<endl;
	if (depthImage.rows > 950)
		type = 0;
	else
		type = 1;

	Mat filterImage, canny, erodeImg, maskErode, blackMask,grad_x,grad_y,abs_grad_x,abs_grad_y,dst,lapalace,abs_lapalace,canny1;
	depthImage.copyTo(blackMask);
	makeMask(depthImage, blackMask, 5, 5, classify, matchLocation,type);
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
	//dst.copyTo(canny);

	for 	(int i = 0; i<canny.rows; i++)
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

	//imshow("漏洞", depthImage);

	drawing.copyTo(depthImage);
	//waitKey();

	return j;
}
