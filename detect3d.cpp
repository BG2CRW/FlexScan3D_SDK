//预处理：滤波+canny
#include "flatulence.hpp"
#include "detect3d.hpp"
#define DEBUG 1
#define PI 3.1415926  
#include <vector>
#include <stack>

class flatulence Flatulence;

using namespace std;
using namespace cv;

/***************************************************************************************
Function:  区域生长算法
Input:     src 待处理原图像 pt 初始生长点 th 生长的阈值条件
Output:    肺实质的所在的区域 实质区是白色，其他区域是黑色
Description: 生长结果区域标记为白色(255),背景色为黑色(0)
Return:    Mat
Others:    NULL
***************************************************************************************/
Mat RegionGrow(Mat src, Point2i pt, int th)
{
	Point2i ptGrowing;                      //待生长点位置  
	int nGrowLable = 0;                             //标记是否生长过  
	int nSrcValue = 0;                              //生长起点灰度值  
	int nCurValue = 0;                              //当前生长点灰度值  
	Mat matDst;// = Mat::zeros(src.size(), CV_8UC1);   //创建一个空白区域，填充为黑色  
	src.copyTo(matDst);
													//生长方向顺序数据  
	int DIR[8][2] = { { -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	vector<Point2i> vcGrowPt;                     //生长点栈  
	vcGrowPt.push_back(pt);                         //将生长点压入栈中  
	matDst.at<uchar>(pt.y, pt.x) = 255;               //标记生长点  
	nSrcValue = src.at<uchar>(pt.y, pt.x);            //记录生长点的灰度值  

	while (!vcGrowPt.empty())                       //生长栈不为空则生长  
	{
		pt = vcGrowPt.back();                       //取出一个生长点  
		vcGrowPt.pop_back();

		//分别对八个方向上的点进行生长  
		for (int i = 0; i<9; ++i)
		{
			ptGrowing.x = pt.x + DIR[i][0];
			ptGrowing.y = pt.y + DIR[i][1];
			//检查是否是边缘点  
			if (ptGrowing.x < 0 || ptGrowing.y < 0 || ptGrowing.x >(src.cols - 1) || (ptGrowing.y > src.rows - 1))
				continue;

			nGrowLable = matDst.at<uchar>(ptGrowing.y, ptGrowing.x);      //当前待生长点的灰度值  

			if (nGrowLable == 0)                    //如果标记点还没有被生长  
			{
				nCurValue = src.at<uchar>(ptGrowing.y, ptGrowing.x);
				if (abs(nSrcValue - nCurValue) < th)                 //在阈值范围内则生长  
				{
					matDst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;     //标记为白色  
					vcGrowPt.push_back(ptGrowing);                  //将下一个生长点压入栈中  
				}
			}
		}
	}
	return matDst.clone();
}

Mat RegionGrow1(Mat src, Point2i pt, int th)
{
	Point2i ptGrowing;                      //待生长点位置  
	int nGrowLable = 0;                             //标记是否生长过  
	int nSrcValue = 0;                              //生长起点灰度值  
	int nCurValue = 0;                              //当前生长点灰度值  
	Mat matDst = Mat::zeros(src.size(), CV_8UC1);   //创建一个空白区域，填充为黑色  
	src.copyTo(matDst);
	//生长方向顺序数据  
	int DIR[8][2] = { { -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },{ 1,1 },{ 0,1 },{ -1,1 },{ -1,0 } };
	vector<Point2i> vcGrowPt;                     //生长点栈  
	vcGrowPt.push_back(pt);                         //将生长点压入栈中  
	matDst.at<uchar>(pt.y, pt.x) = 255;               //标记生长点  
	nSrcValue = src.at<uchar>(pt.y, pt.x);            //记录生长点的灰度值  

	while (!vcGrowPt.empty())                       //生长栈不为空则生长  
	{
		pt = vcGrowPt.back();                       //取出一个生长点  
		vcGrowPt.pop_back();

		//分别对八个方向上的点进行生长  
		for (int i = 0; i<9; ++i)
		{
			ptGrowing.x = pt.x + DIR[i][0];
			ptGrowing.y = pt.y + DIR[i][1];
			//检查是否是边缘点  
			if (ptGrowing.x < 0 || ptGrowing.y < 0 || ptGrowing.x >(src.cols - 1) || (ptGrowing.y > src.rows - 1))
				continue;

			nGrowLable = matDst.at<uchar>(ptGrowing.y, ptGrowing.x);      //当前待生长点的灰度值  

			if (nGrowLable == 0)                    //如果标记点还没有被生长  
			{
				nCurValue = src.at<uchar>(ptGrowing.y, ptGrowing.x);
				if (abs(nSrcValue - nCurValue) < th)                 //在阈值范围内则生长  
				{
					matDst.at<uchar>(ptGrowing.y, ptGrowing.x) = 255;     //标记为白色  
					vcGrowPt.push_back(ptGrowing);                  //将下一个生长点压入栈中  
				}
			}
		}
	}
	return matDst.clone();
}

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
		cout << "maxvalue: " << maxValue << endl;
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
	int flag = 0;
	int face;
	cv::Mat src;
	cv::Mat image2D, image3D;
	double scale, tempx, tempy, x2D,  y2D, x3D, y3D;
	scale = 1.35;
	Point matchLocation_2Dapple[2];
	Point matchLocation_3Dapple[2];
	string path_2Derror = "D:/model/model_2Drerror.jpg";
	string path_2Dapple = "D:/model/model_2Dapple.jpg";
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
	findModel(image2D, path_2Dapple, matchLocation_2Dapple, 0.8);
	if (matchLocation_2Dapple[0].x != 0)
	{
		/*cout << "Apple face" << endl;
		cout << "matchLocation_2Dapple" << matchLocation_2Dapple[0].x << endl;*/
		face = 0;
		flag = findlogo(image3D, face, matchLocation_3Dapple);
	}
	else
	{
		findModel(image2D, path_2Derror, matchLocation_2Dapple, 0.8);
		/*cout << "Error face" << endl;
		cout << "matchLocation_2Dapple:  " << matchLocation_2Dapple[0].x<< endl;*/
		face = 1;
		flag = findlogo(image3D, face, matchLocation_3Dapple);
	}

	/*findModel(image2D, path_2Dapple, matchLocation_2Dapple, 0.4);
	findModel(image3D, path_3Dapple1, matchLocation_3Dapple, 0.8);*/
	/*if (matchLocation_3Dapple[0].x == 0)
	{
		cout << "matchLocation1: " << matchLocation_3Dapple[0].x << endl;
		findModel(image3D, path_3Dapple2, matchLocation_3Dapple, 0.8);
		if (matchLocation_3Dapple[0].x == 0)
		{
			cout << "matchLocation2: " << matchLocation_3Dapple[0].x << endl;
			findModel(image3D, path_3Derror2, matchLocation_3Dapple, 0.4);
			findModel(image2D, path_2Derror, matchLocation_2Dapple, 0.8);
			cout << "matchLocation2.2: " << matchLocation_3Dapple[0].x << endl;
			if (matchLocation_3Dapple[0].x == 0)
			{
				cout << "matchLocation3: " << matchLocation_3Dapple[0].x << endl;
				findModel(image3D, path_3Derror1, matchLocation_3Dapple, 0.4);
				if (matchLocation_3Dapple[0].x == 0)
				{
					flag = 1;
				}
			}
		}
	}*/
	resize(image2D, image2D, Size(image2D.cols / scale, image2D.rows / scale), 0, 0, INTER_LINEAR);
	matchLocation_2Dapple[0].x /= scale;
	matchLocation_2Dapple[0].y /= scale;
	matchLocation_2Dapple[1].x /= scale;
	matchLocation_2Dapple[1].y /= scale;
	if (flag != 1)
	{
	x2D = matchLocation_2Dapple[0].x;
	y2D = matchLocation_2Dapple[0].y;
	/*rectangle(image2D, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(255, 255, 255), 2, 8, 0);
	rectangle(image3D, matchLocation_3Dapple[0], matchLocation_3Dapple[1], Scalar(0, 0, 255), 2, 8, 0);*/
	/*rectangle(erodeBinary, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(0, 0, 255), 2, 8, 0);*/
	/*image2D = 255 - image2D;
	rectangle(image2D, matchLocation_2Dapple[0], matchLocation_2Dapple[1], Scalar(0, 0, 255), 2, 8, 0);
	rectangle(image3D, matchLocation_3Dapple[0], matchLocation_3Dapple[1], Scalar(0, 0, 255), 2, 8, 0);
	cv::imshow("image2D", image2D);*/
	/*cv::imshow("image3D", erodeBinary);*/
	x3D = matchLocation_3Dapple[0].x;
	y3D = matchLocation_3Dapple[0].y;
	//image2D = 255 - image2D;
	/*cv::imshow("image2D2", image2D);
	cv::imshow("image3D2", image3D);
	waitKey();*/
	for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				tempx = i - y3D + y2D;
				tempy = j - x3D + x2D;
				if (tempx < 0 ||tempy < 0|| tempx > image2D.rows|| tempy > image2D.cols)
				{
					break;
				}
				if (image2D.at<uchar>(tempx, tempy) > 50)
					src.at<uchar>(i, j) = 0;
			}
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
cv::Mat gamaTest(Mat img)

{

	imshow("原始图像", img);
	Mat imgGamma;
	img.copyTo(imgGamma);

	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			if (img.at<int>(i, j) == 0)
			{
				img.at<int>(i, j) = 127;
			}

			imgGamma.at<int>(i, j) = (img.at<int>(i, j) - 127) * 3 + 127;
		}

	}

	//归一化

	normalize(imgGamma, imgGamma, 0, 255, CV_MINMAX);

	//转换为8 bit图像显示

	convertScaleAbs(imgGamma, imgGamma);

	//imshow("伽马增强效果", imgGamma);
	//waitKey();
	return imgGamma;
}


int detect3d::check3d(cv::Mat depthImage,cv::Mat silk2D)
{
	Mat canny, blackMask,grad_x,grad_y,abs_grad_x,abs_grad_y,dst,lapalace,abs_lapalace,canny1,imginter,imgunion,seedgrow;
	/*imshow("Src", depthImage);*/
	/*depthImage = gamaTest(depthImage);
	imshow("gamaTest", depthImage);
	waitKey();*/
	depthImage.copyTo(blackMask);
	//imshow("gamaTest2", blackMask);
	makeMask(depthImage, blackMask, silk2D, 5, 4);
	/*imshow("blackMask",blackMask);*/

	Sobel(depthImage, grad_x, CV_16S, 0, 1, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);
	/*imshow("x direction Sobel", abs_grad_x);*/

	Sobel(depthImage, grad_y, CV_16S, 1, 0, 3, 1, 1, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);
	/*imshow("y direction Sobel", abs_grad_y);*/

	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, dst);
	/*imshow("all direction Sobel", dst);*/

	Canny(depthImage, canny1, 10, 110, 3);
	/*imshow("canny1",canny1);*/
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
	//imshow("thresold", canny);
	Mat element = getStructuringElement(MORPH_RECT, Size(1, 1), Point(-1, -1));
	Mat Mask;
	erode(255 - canny, Mask, element);
	erode(Mask, Mask, element);
	/*imshow("Mask", Mask);*/
	Mask.copyTo(imginter);
	Mask.copyTo(imgunion);

	for (int i = 0; i<Mask.rows; i++)
	{
		for (int j = 0; j<Mask.cols; j++)
		{
 			imgunion.at<uchar>(i, j) = Mask.at<uchar>(i, j) / 255 * blackMask.at<uchar>(i, j);
		}
	}
	//imgunion = 255 - imgunion;
	/*imshow("imgunion", imgunion);*/

	Point2i seed;
	//seed.x = 1;
	//seed.y = 1;
	for (int i = 0; i<canny.rows; i++)
	{
		for (int j = 0; j<canny.cols; j++)
		{
			if (Mask.at<uchar>(i, j) == 0 && blackMask.at<uchar>(i, j) == 0)
			{
				imginter.at<uchar>(i, j) = 0;
				seed.x = j; 
				seed.y = i;
				imgunion = RegionGrow(imgunion, seed, 10);
			}
			else
			{
				imginter.at<uchar>(i, j) = 255;
			}
		}
	}
	/*imshow("imginter", imginter);
	imshow("seedgrow", imgunion);*/
	imgunion.copyTo(seedgrow);
	ConnectEdge(Mask);
	Mat dep1;
	depthImage.copyTo(dep1);
	for (int i = 0; i<seedgrow.rows; i++)
	{
		for (int j = 0; j<seedgrow.cols; j++)
		{
			if (seedgrow.at<uchar>(i, j) == 0 )
			{
				seed.x = j;
				seed.y = i;
				dep1 = RegionGrow1(dep1, seed, 0);
			}
		}
	}
	/*imshow("hua", dep1);*/

	Mat element1 = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	erode(seedgrow, seedgrow, element1);
	dilate(seedgrow, seedgrow, element1);
	/*imshow("seedgrowerode", seedgrow);*/
	rectangle(seedgrow, Point(0, 0), Point(seedgrow.cols, 10), Scalar(255, 255, 255), CV_FILLED, 8, 0);
	rectangle(seedgrow, Point(0, 0), Point(10, seedgrow.rows), Scalar(255, 255, 255), CV_FILLED, 8, 0);
	rectangle(seedgrow, Point(0, seedgrow.rows-10), Point(seedgrow.cols, seedgrow.rows), Scalar(255, 255, 255), CV_FILLED, 8, 0);
	rectangle(seedgrow, Point(seedgrow.cols-10, 0), Point(seedgrow.cols, seedgrow.rows), Scalar(255, 255, 255), CV_FILLED, 8, 0);
	/*imshow("seed", seedgrow);*/
	int x1[20]; int y1[20]; int area1[20]; int length1[20]; int depth1[20]; int num; int xmax[20]; int ymax[20]; int depthmin[20];
	judge(depthImage,seedgrow, x1, y1,xmax,ymax,area1, length1, depth1,depthmin,num);
	int j = 1;
	return j;
}

int detect3d::errorReport(cv::Mat imgdepthVert, cv::Mat imgdepthHor, cv::Mat silk2D)
{
	int report;
	report = Flatulence.flatulenceCheck(imgdepthVert, 127, 2.3, 240, 35);
	report = Flatulence.flatulenceCheck(imgdepthHor, 127, 2.3, 240, 35);
	check3d(imgdepthVert, silk2D);
	check3d(imgdepthHor, silk2D);
	return report;
}

int detect3d::findlogo(cv::Mat image3D, int face, Point* matchLocation_3Dapple)
{
	int couter = 0;
	FILE* fh;
	char path_3Dapple[200], path_3Derror[200], str0[2];
	char path_3Dappleprix[] = "D:/model/model_apple";
	char path_3Derrorprix[] = "D:/model/model_error";
	char path_suffix[] = ".jpg";
	sprintf(str0, "%2d", couter);
	sprintf(path_3Dapple, "%s%s%s", path_3Dappleprix, str0, path_suffix);
	sprintf(path_3Derror, "%s%s%s", path_3Derrorprix, str0, path_suffix);

	if (face == 0)	//Apple face
	{
		while (1)
		{
			couter++;
			sprintf(str0, "%d", couter);
			sprintf(path_3Dapple, "%s%s%s", path_3Dappleprix, str0, path_suffix);
			fh = fopen(path_3Dapple, "r");
			if (fh == NULL)
			{
				std::cout << "can not open this file" << endl;
				return 1;
			}
			else
			{
				findModel(image3D, path_3Dapple, matchLocation_3Dapple, 0.6);
				if (matchLocation_3Dapple[0].x != 0)
				{
					cout << "in: " << matchLocation_3Dapple[0].x << " " << matchLocation_3Dapple[0].y << endl;
					return 0;
				}
			}
		}
	}

	if (face == 1)   //Error face
	{
		while (1)
		{
			couter++;
			sprintf(str0, "%d", couter);
			sprintf(path_3Derror, "%s%s%s", path_3Derrorprix, str0, path_suffix);
			fh = fopen(path_3Derror, "r");
			if (fh != NULL)
			{
				findModel(image3D, path_3Derror, matchLocation_3Dapple, 0.6);
				if (matchLocation_3Dapple[0].x != 0)
				{
					cout << "in: " << matchLocation_3Dapple[0].x << " " << matchLocation_3Dapple[0].y << endl;
					return 0;
				}
			}
			else
			{
				std::cout << "can not open this file" << endl;
				return 1;
			}
		}
	}
}
void detect3d::judge(cv::Mat src,cv::Mat image, int* x, int* y, int* xmax,int* ymax,int* area, int* length, int *depthmax, int* depthmin, int& num)
{
	cv::Mat seedgrow;
	image.copyTo(seedgrow);
	vector<vector<Point> > contours;
	vector<vector<Point> > contours1;
	vector<Vec4i> hierarchy;
	findContours(seedgrow, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
	Mat drawing = Mat::zeros(seedgrow.size(), CV_8U);
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

	cv::Mat element15(3, 3, CV_8U, cv::Scalar(1));
	cv::Mat close;
	cv::morphologyEx(drawing, close, cv::MORPH_CLOSE, element15);
	/*imshow("drawing", drawing);*/
	//waitKey();
	findContours(close, contours1, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(-1, -1));
	vector<Vec4i> hierarchy1;
	vector<Point2f> mc(contours1.size());//center of mass
										//计算轮廓矩
	vector<Moments> mu(contours1.size());
	for (int i = 0; i < contours1.size(); i++)
	{
		mu[i] = moments(contours1[i], false);
	}
	cv::Mat zeroimg;
	/*findContours(close, contours1, hierarchy1, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));*/
	/*imshow("close", close);*/
	num = contours1.size();
	for (int i = 0; i < contours1.size(); i++)
	{
		zeroimg = Mat::zeros(close.size(), CV_8U);
		drawContours(zeroimg, contours1, i, Scalar(255, 255, 255), FILLED, 8, hierarchy1, 0, Point());
		Moments moms = moments(Mat(contours1[i]));
		area[i] = moms.m00; 
		length[i]=arcLength(contours1[i], true);
		mc[i] = Point2d(mu[i].m10 / mu[i].m00, mu[i].m01 / mu[i].m00);
		x[i] = mc[i].x;
		y[i] = mc[i].y;
		int depthmax1 = 0;
		int depthmin1 = 255;
		int tempx = 0;
		int tempy = 0;
		for (int i = 0; i<zeroimg.rows; i++)
		{
			for (int j = 0; j<zeroimg.cols; j++)
			{
				if (zeroimg.at<uchar>(i, j)==255)
				{ 
					if (src.at<uchar>(i, j) > depthmax1)
					{
						depthmax1 = src.at<uchar>(i, j);
						tempx = i;
						tempy = j;
					}
					if (src.at<uchar>(i, j) < depthmin1)
					{
						depthmin1 = src.at<uchar>(i, j);
					}
				}
			}
		}
		xmax[i] = tempx;
		ymax[i] = tempy;
		depthmax[i] = depthmax1;
		depthmin[i] = depthmin1;
	}

	for (int i = 0; i < num; i++)
	{
		if (x[i] < 950)
		{
			if (depthmax[i] - depthmin[i] > 20)
			{
				drawContours(src, contours1, i, Scalar(0, 0, 255), FILLED, 8, hierarchy1, 0, Point());;
				cout << "The " << i + 1 << "th " << "X: " << x[i] << " Y: " << y[i] << " length: " << length[i] << " area: " << area[i] << " maxdepth: " << depthmax[i] << " mindepth: " << depthmin[i] << endl;
				if (area[i] > 300)
				{
					cout << "NG!" << endl;
			    }
				double r = length[i] / (2 * 3.1415);
				if (3.1415 * r * r > 1.8 * area[i])
				{
					cout << "It is a scratch!" << endl;
				}
				else
				{
					cout << "It is a convex/concave point!" << endl;
				}
			}
			
		}
	}
	imshow("result", src);
	/*imshow("zeroimg", zeroimg);*/
	//j = 0;
	//int m = 0;
	//int resultID = 0;
	//for (int i = 0; i < contours1.size(); i++)
	//{
	//	Moments moms = moments(Mat(contours1[i]));
	//	double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
	//	double templength = arcLength(contours[i], true);   //如果面积超出了设定的范围，则不再考虑该斑点  

	//	double area1 = contourArea(contours1[i]);
	//	if (area > 500)
	//	{
	//		resultID++;
	//	}
	//	if (area >27 && area < 100000)
	//	{
	//		drawContours(depthImage, contours1, i, Scalar(0, 0, 255), FILLED, 8, hierarchy1, 0, Point());
	//		length.push_back(templength);
	//		cout << "The " << i << "th scratch Length: " << templength / 2 << "  th area: " << area << endl;
	//		j = j + 1;

	//	}
	//	else if (area >= 0 && area <= 27)
	//	{
	//		drawContours(depthImage, contours1, i, Scalar(255, 0, 0), FILLED, 8, hierarchy1, 0, Point());
	//		cout << "The " << i << "th dot Length: " << templength / 2 << "  th area: " << area << endl;
	//		m = m + 1;
	//	}
	//}
	//if (resultID)
	//{
	//	cout << "NG" << endl;
	//}
	//char t[256];
	//cout << j << endl;
	//imshow("Result", depthImage);
	//waitKey();
	//drawing.copyTo(depthImage);
}

