//Scratch detection (No Chinese)
#include "detect2d.hpp"

using namespace std;
using namespace cv;

void onChangeTrackBar(int pos, void* data);
void onChangeTrackBarCanny(int pos, void* data);

cv::Mat imageTemp;

int detect2d::scratchCheck(cv::Mat image)
{
	//cvtColor(image, image, CV_RGB2GRAY, 0);
	batteryKind = 2;
	if (batteryKind == 1)//Iphone8 Battery
	{
		//007
		//minRow = 380;
		//maxRow = 870;
		//minCol = 190;
		//maxCol = 1400;
		//016
		//minRow = 350;
		//maxRow = 840;
		//minCol = 200;
		//maxCol = 1410;
		//024
		//minRow = 370;
		//maxRow = 870;
		//minCol = 175;
		//maxCol = 1385;
		//For test
		minRow = 150;
		maxRow = 1050;
		minCol = 50;
		maxCol = 1550;
	}
	if (batteryKind == 2)//Iphone8+ Battery
	{
		//minRow = 210;
		//maxRow = 1000;
		//minCol = 270;
		//maxCol = 1245;
		//For test
		minRow = 150;
		maxRow = 1050;
		minCol = 50;
		maxCol = 1550;
	}
	if (batteryKind == 3)//HW Battery
	{
		minRow = 210;
		maxRow = 1000;
		minCol = 270;
		maxCol = 1245;
	}
	cv::Mat image2, Mask, imageBlack;
	imshow("Image", image);
	imwrite("D:/660image.jpg", image);
	image.copyTo(image2);
	image.copyTo(imageBlack);
	image.copyTo(imageTemp);

	//检测黑色发亮
	int blackID = blackDetect(imageBlack);
	cout << "Result ID for Black detection: " << blackID << ". (1 for OK,2 for NG)" << endl;
	//imshow("Black", imageBlack);
	//imshow("BlackModel", imageTemp);

	//Make the adaptive model
	cv::Mat adpModel, adpROI;
	//自适应方法做丝印ROI
	bitwise_not(image2, adpROI);
	adaptiveThreshold(adpROI, adpROI, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);
	Mat elementAdp = getStructuringElement(MORPH_RECT, Size(6, 6));
	dilate(adpROI, adpROI, elementAdp);
	//imshow("One step adp", adpROI);
	//imwrite("D:/661modeladp.jpg", adpROI);
	//利用自适应ROI制作丝印去除模版
	adpModel = silkMask(image2, adpROI);
	imshow("adpModel0", imageTemp);
	imwrite("D:/661model0.jpg", imageTemp);
	imshow("adpModel", adpModel);
	imwrite("D:/661model.jpg", adpModel);
	waitKey();

	//Prehandle of the image
	Mask = preProcess(image2);

	//Cut the silk model
	bitwise_or(imageBlack, adpModel, adpModel);
	bitwise_not(adpModel, adpModel);
	bitwise_and(Mask, adpModel, Mask);
	imshow("Cut the silk", Mask);
	imwrite("D:/663cutModel.jpg", Mask);

	//cut the edge violently
	edgeCut(Mask);
	//imshow("Cut the edge", Mask);
	imwrite("D:/664cutEdge.jpg", Mask);

	//Outstand the defect
	Mat element11 = getStructuringElement(MORPH_RECT, Size(2, 2));
	erode(Mask, Mask, element11);
	dilate(Mask, Mask, element11);

	//Show all defects
	cv::Mat imageShow, maskShow;
	image.copyTo(imageShow);
	Mask.copyTo(maskShow);
	showDefect(imageShow, maskShow);

	//Detect the Liquid
	cv::Mat imageLiquid, maskLiquid;
	image.copyTo(imageLiquid);
	Mask.copyTo(maskLiquid);
	int liquidID = liquidDetect(imageLiquid, maskLiquid);
	cout << "Result ID for Liquid detection: " << liquidID << ". (1 for OK,2 for NG)" << endl;

	//Detect the Al
	cv::Mat imageAl, maskAl;
	image.copyTo(imageAl);
	Mask.copyTo(maskAl);
	int alID = alDetect(imageAl, maskAl);
	cout << "Result ID for Al detection: " << alID << ". (1 for OK,2 for NG)" << endl;

	//Detect the Scratch
	cv::Mat imageScratch, maskScratch;
	image.copyTo(imageScratch);
	Mask.copyTo(maskScratch);
	int scratchID = scratchDetect(imageScratch, maskScratch);
	cout << "Result ID for Scratch detection: " << scratchID << ". (1 for OK,2 for NG)" << endl;

	waitKey(0);

	int errorID = 0;
	return errorID;
}

int detect2d::liquidDetect(cv::Mat origin, cv::Mat inputImage)
{
	int resultID = 1;

	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(10, 10));
	erode(inputImage, inputImage, element33);
	dilate(inputImage, inputImage, element44);
	imshow("liquid0", inputImage);

	//Find the damages
	vector<vector<Point>> contours;//定义轮廓
	vector<vector<Point>> contoursvalue;  //选取符合大小的轮廓
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
	//findContours函数寻找轮廓
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//寻找黑白点数量
	int liquidNum = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		int whiteNum = 0, blackNum = 0;
		for (int j = 0; j < contours[i].size(); j++)
		{
			if (origin.at<uchar>(contours[i][j].y, contours[i][j].x) > 170)
			{
				whiteNum++;
			}
			if (origin.at<uchar>(contours[i][j].y, contours[i][j].x) < 100)
			{
				blackNum++;
			}
		}
		if (whiteNum > 1 || blackNum > 1)
		{
			//cout << whiteNum << " and " << blackNum << endl;
		}
		if (whiteNum > 3 && blackNum > 3)
		{
			liquidNum++;
			contoursvalue.push_back(contours[i]);
		}
	}
	for (int i = 0; i < contoursvalue.size(); i++)
	{
		drawContours(origin, contoursvalue, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
	}
	if (contoursvalue.size() > 0)
		resultID = 2;
	cout << "There are " << liquidNum << " liquid defects." << endl;
	imshow("liquid", origin);
	imwrite("F:/liquidResult.jpg", origin);

	return resultID;
}

int detect2d::blackDetect(cv::Mat inputImage)
{
	int resultID = 1;
	//二值化
	threshold(inputImage, inputImage, 85, 255, THRESH_BINARY_INV);
	//设置ROI
	for (int j = 0; j<inputImage.rows; j++)
	{
		uchar* black = inputImage.ptr<uchar>(j);
		for (int i = 0; i<inputImage.cols; i++)
		{
			if (j<minRow - 0 || j>maxRow + 0 || i<minCol || i>maxCol + 0)
			{
				black[i] = 0;
			}
		}
	}
	//处理图像
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(12, 12));
	erode(inputImage, inputImage, element1);
	dilate(inputImage, inputImage, element2);

	//Find the black
	vector<vector<Point> > contours;//定义轮廓
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
	//findContours函数寻找轮廓
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	//计算轮廓面积
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
		if (area > 5)
		{
			//drawContours(imageTemp, contours, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
			j = j + 1;
		}
	}
	if (j > 0)
	{
		cout << "There are " << j << " black shine." << endl;
		resultID = 2;
	}

	return resultID;
}

int detect2d::scratchDetect(cv::Mat origin, cv::Mat inputImage)
{
	int resultID = 1;
	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element88 = getStructuringElement(MORPH_RECT, Size(6, 6));
	Mat element55 = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(inputImage, inputImage, element33);
	imshow("scratch000", inputImage);
	dilate(inputImage, inputImage, element88);
	imshow("scratch00", inputImage);
	erode(inputImage, inputImage, element55);
	imshow("scratch0", inputImage);
	imwrite("F://Scratch.jpg", inputImage);

	//Find the damages
	vector<vector<Point>> contours;//定义轮廓
	vector<vector<Point>> contoursvalue;  //选取符合大小的轮廓
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
	//findContours函数寻找轮廓
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	//计算轮廓面积及周长
	vector<double> length;
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;								 //零阶矩即为二值图像的面积  double area = moms.m00;  
		double templength = arcLength(contours[i], true);    //计算周长

		if (area > 60 && area < 10000)	//面积筛选
		{
			//cout << "are: " << area << endl;
			double r = templength / (2 * 3.1415);
			if (3.1415 * r * r > 1.8 * area && templength > 40)
			{
				contoursvalue.push_back(contours[i]);
				length.push_back(templength);
				j++;
				//cout << "The area: " << area << " and templength: " << templength << endl;
			}
		}
	}
	for (int i = 0; i < contoursvalue.size(); i++)
	{
		drawContours(origin, contoursvalue, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
	}
	imshow("scratch", origin);
	imwrite("F:/scratchResult.jpg", origin);

	//计算实际长度,判断长中短划痕数量
	double scale = 1600.0/116.5;
	int longScratch = 0, midScratch = 0, shortScratch = 0;
	for (int g = 0; g < length.size(); g++)
	{
		length[g] /= scale;
		if (length[g] / 2 > 1)
		{
			if (length[g] / 2 > 10)
			{
				if (length[g] / 2 > 20)
					longScratch++;
				else
					midScratch++;
			}
			else
			{
				shortScratch++;
			}
		}
	}
	//cout << couter << endl;
	if (longScratch * 1 + midScratch*0.5 + shortScratch*0.33 > 1)
	{
		cout << "Scratch Long:" << longScratch << ", Mid: " << midScratch << ", Short: " << shortScratch << ". NG!" << endl;
		resultID = 2;
	}

	return resultID;
}

int detect2d::alDetect(cv::Mat origin, cv::Mat inputImage)
{
	int resultID = 1;
	Mat elementAl = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(inputImage, inputImage, elementAl);
	//dilate(InputImage, InputImage, elementAl);
	imshow("Al0", inputImage);
	imwrite("F:/Al.jpg", inputImage);

	vector<vector<Point>> contours;//定义轮廓
	vector<vector<Point>> contoursfinal;
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
	//findContours函数寻找轮廓
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//检测漏铝
	vector<double> mean;
	for (int Size = 0; Size < contours.size(); Size++)
	{
		double value = 0;
		double meanValue = 0;

		for (int Size0 = 0; Size0 < contours[Size].size(); Size0++)
		{
			value += origin.at<uchar>(contours[Size][Size0].y, contours[Size][Size0].x);
		}
		meanValue = value / contours[Size].size();
		mean.push_back(meanValue);

		if (meanValue > 190)	//设置阈值
		{
			//cout << "第" << Size + 1 << "个轮廓漏铝: " << meanValue << endl;
			contoursfinal.push_back(contours[Size]);
		}
	}
	//sort(mean.begin(), mean.end(), greater<double>());
	//cout << "最大值: " << mean[0] << endl;
	for (int i = 0; i < contoursfinal.size(); i++)
	{
		drawContours(origin, contoursfinal, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
	}
	imshow("Al", origin);
	imwrite("F:/AlResult.jpg", origin);

	if (contoursfinal.size() > 0)
		resultID = 2;

	return resultID;
}

void detect2d::showDefect(cv::Mat finalShow, cv::Mat inputImage)
{
	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(8, 8));
	erode(inputImage, inputImage, element33);
	dilate(inputImage, inputImage, element44);
	("All defects", inputImage);
	imwrite("D:/665defect.jpg", inputImage);

	//Find the damages
	vector<vector<Point> > contours;//定义轮廓
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
	//findContours函数寻找轮廓
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	//计算轮廓面积
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
								   //如果面积超出了设定的范围，则不再考虑该斑点  
		if (area > 80 && area < 10000)
		{
			drawContours(finalShow, contours, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
			j = j + 1;
		}
	}
	char t[256];
	snprintf(t, sizeof(t), "%01d", j);
	string s = t;
	string txt = "NG number : " + s;
	putText(finalShow, txt, Point(20, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);

	if (2 > 1)//mark the edge
	{
		for (int j = 0; j<finalShow.rows; j++)
		{
			uchar* datad = finalShow.ptr<uchar>(j);
			for (int i = 0; i<finalShow.cols; i++)
			{
				if (j == minRow || j == maxRow || i == minCol || i == maxCol)
				{
					datad[i] = 0;
				}
			}
		}
	}

	imshow("drawing image", finalShow);
	imwrite("D:/666final.jpg", finalShow);
}

void detect2d::edgeCut(cv::Mat inputImage)
{
	for (int j = 0; j<inputImage.rows; j++)
	{
		uchar* data1 = inputImage.ptr<uchar>(j);
		for (int i = 0; i<inputImage.cols; i++)
		{
			if (j<minRow || j>maxRow || i<minCol || i>maxCol)
			{
				data1[i] = 0;
			}
			//if (j==minRow || j==maxRow || i==minCol || i==maxCol)
			//{
			//	data1[i] = 255;
			//}
		}
	}
}

cv::Mat detect2d::silkMask(cv::Mat inputImage, cv::Mat adpROI)
{
	//1.二值化
	surfaceIndex = 1;
	Mat binary;
	threshold(inputImage, binary, 240, 255, THRESH_BINARY);
	//adaptiveThreshold(img, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);
	//1.2.Set the ROI
	for (int j = 0; j<binary.rows; j++)
	{
		uchar* bina = binary.ptr<uchar>(j);
		for (int i = 0; i<binary.cols; i++)
		{
			if (j<minRow-10 || j>maxRow+10 || i<minCol || i>maxCol+50)
			{
				bina[i] = 0;
			}
		}
	}
	imshow("Binary", binary);

	//2.膨胀和腐蚀
	Mat dilate1, erode1, dilate2, erode2, dilate3;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(19, 19));
	//控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
	Mat element2 = getStructuringElement(MORPH_RECT, Size(19, 19));
	Mat element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat element4 = getStructuringElement(MORPH_RECT, Size(3, 3));
	//膨胀一次，让轮廓突出
	dilate(binary, dilate1, element2);
	//腐蚀一次，去掉细节，表格线等。这里去掉的是竖直的线
	erode(dilate1, erode1, element1);
	//再次膨胀，让轮廓明显一些
	dilate(erode1, dilate2, element2);
	erode(dilate2, erode2, element1);
	//再次膨胀，让轮廓明显一些
	dilate(erode2, dilate3, element3);

	////显示和保存
	//imshow("腐蚀和膨胀0", erode2);
	imshow("腐蚀和膨胀", dilate3);
	//imwrite("D:/111/2腐蚀和膨胀.jpg", dilate3);

	//3.筛选正确的ROI
	//3.1.查找轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilate3, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//3.2.筛选那些面积过小和过大的
	//cout << "Num of contours: " << contours.size() << endl;
	vector<vector<Point>> contours_size8000;
	for (int i = 0; i < contours.size(); i++)
	{
		//计算当前轮廓的面积
		double area = contourArea(contours[i]);
		//cout << i << "The area is: " << area << endl;

		//面积小于1000和大于600000的全部筛选掉
		if (area < 1000 || area > 600000)
			continue;
		else
		{
			if (area > 90000)
			{
				surfaceIndex = 2;
			}
			contours_size8000.push_back(contours[i]);
			//drawContours(imageTemp, contours, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
		}
	}
	if (surfaceIndex == 1)
	{
		cout << "This is Engilsh face!1111111111111111" << endl;
	}
	else
		cout << "This is Chinese face!2222222222222222" << endl;
	//cout << "size:" << contours_size8000.size() << endl;
	Mat ContoursMast(dilate3.size(), CV_8U, Scalar(0));
	drawContours(ContoursMast, contours_size8000, -1, Scalar(255), CV_FILLED);


	//暴力制作喷码模版
	if (batteryKind == 2 && surfaceIndex == 2)
	{
		int min1, max1, min2, max2;
		min1 = 400;
		max1 = 1200 - 466;
		min2 = 1600 - 300;
		max2 = 1600 - 133;
		Mat printing(inputImage.size(), CV_8U, Scalar(0));
		for (int j = min1; j<max1; j++)
		{
			uchar* dataP = printing.ptr<uchar>(j);
			uchar* dataI = inputImage.ptr<uchar>(j);
			for (int i = min2; i<max2; i++)
			{
				if (dataI[i] > 240)
				{
					dataP[i] = 255;
				}
			}
		}
		Mat elementP25 = getStructuringElement(MORPH_RECT, Size(18, 18));
		Mat elementP5 = getStructuringElement(MORPH_RECT, Size(25, 25));
		//dilate(printing, printing, elementP25);
		//erode(printing, printing, elementP25);
		dilate(printing, printing, elementP5);

		imshow("priting", printing);

		bitwise_or(ContoursMast, printing, ContoursMast);
	}
	

	////3.3.与自适应ROI结合
	//bitwise_and(ContoursMast, adpROI, ContoursMast);

	//vector<vector<Point>> contoursROI,contoursFinal;
	//vector<Vec4i> hierarchyROI;
	//findContours(ContoursMast, contoursROI, hierarchyROI, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//for (int i = 0; i < contoursROI.size(); i++)
	//{
	//	double area = contourArea(contoursROI[i]);
	//	//面积小于100和大于600000的全部筛选掉
	//	if (area < 40 || area > 600000)
	//		continue;
	//	else
	//	{
	//		contoursFinal.push_back(contoursROI[i]);
	//		drawContours(imageTemp, contoursROI, i, Scalar(0), FILLED, 8, hierarchyROI, 0, Point());
	//	}
	//}
	//cv::Mat finalROI(dilate3.size(), CV_8U, Scalar(0));
	//drawContours(finalROI, contoursFinal, -1, Scalar(255), CV_FILLED);
	//imshow("最终ROI", finalROI);




	//4.得到丝印模版
	//4.1取交集+二值化+膨胀
	Mat RoiImg, ROIImg1, ROIImg2;
	bitwise_and(ContoursMast, inputImage, RoiImg);
	//bitwise_and(finalROI, inputImage, RoiImg);
	threshold(RoiImg, ROIImg1, 200, 255, THRESH_BINARY);
	dilate(ROIImg1, ROIImg2, element4);

	for (int j = 0; j<ROIImg2.rows; j++)
	{
		uchar* data = ROIImg2.ptr<uchar>(j);
		uchar* data111 = imageTemp.ptr<uchar>(j);
		for (int i = 0; i<ROIImg2.cols; i++)
		{
			if (data[i] > 0)
			{
				data111[i] = 0;
			}
		}
	}

	////4.2显示和保存
	//cvNamedWindow("ROI", WINDOW_NORMAL);
	//imshow("ROI", ContoursMast);
	//imwrite("D:/111/ROI.jpg", ContoursMast);
	//cvNamedWindow("ROIImg", WINDOW_NORMAL);
	//imshow("ROIImg", RoiImg);
	//imwrite("D:/111/ROIImg.jpg", RoiImg);
	//cvNamedWindow("ROIImg1", WINDOW_NORMAL);
	//imshow("ROIImg1", ROIImg1);
	//imwrite("D:/111/ROIImg1.jpg", ROIImg1);
	//cvNamedWindow("ROIImg2", WINDOW_NORMAL);
	//imshow("ROIImg2", ROIImg2);
	//imwrite("D:/111/ROIImg2.jpg", ROIImg2);

	//waitKey();
	return ROIImg2;
}

cv::Mat detect2d::preProcess(cv::Mat inputImage)
{
	cv::Mat canny, Mask;
	/*
	namedWindow("Canny");
	imshow("Canny", image2);
	int valueCanny = 0;
	createTrackbar("pos", "Canny", &valueCanny, 300, onChangeTrackBarCanny, &image2);
	waitKey();
	*/
	Canny(inputImage, canny, 30, 77);
	//imshow("canny",canny);
	for (int j = 0; j<inputImage.rows; j++)
	{
		uchar* data = inputImage.ptr<uchar>(j);
		for (int i = 0; i<inputImage.cols; i++)
		{
			data[i] = 255 - data[i];
		}
	}
	/*
	namedWindow("dyn_threshold");
	imshow("dyn_threshold", image2);
	int value = 0;
	createTrackbar("pos", "dyn_threshold", &value, 2550, onChangeTrackBar, &image2);
	waitKey();
	*/
	adaptiveThreshold(inputImage, Mask, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 6 * 2 + 1, 8);
	//imshow("Mask", Mask);

	bitwise_or(Mask, canny, Mask);
	//imshow("Mask2", Mask);
	imwrite("D:/662preHandle.jpg", Mask);

	return Mask;
}

void onChangeTrackBar(int pos, void* data)
{
	// 强制类型转换
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat dstImage;
	// 根据滑动条的值进行二值化
	//cv::threshold(srcImage, dstImage, pos, 255, 0);
	adaptiveThreshold(srcImage, dstImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, pos * 2 + 1, 8);
	cv::imshow("dyn_threshold", dstImage);
}
void onChangeTrackBarCanny(int pos, void* data)
{
	// 强制类型转换
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat canny;
	cv::Canny(srcImage, canny, pos, 77);
	cv::imshow("Canny", canny);
}
