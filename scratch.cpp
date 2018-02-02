//Scratch detection (No Chinese)
#include "scratch.hpp"

using namespace std;
using namespace cv;

cv::Mat scratch::silkMask(cv::Mat inputImage)
{
	//1.二值化
	Mat binary;
	threshold(inputImage, binary, 240, 255, THRESH_BINARY);
	//adaptiveThreshold(img, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);

	//2.膨胀和腐蚀
	Mat dilate1, erode1, dilate2, erode2, dilate3;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(25, 25));
	//控制高度设置可以控制上下行的膨胀程度，例如3比4的区分能力更强,但也会造成漏检
	Mat element2 = getStructuringElement(MORPH_RECT, Size(25, 25));
	Mat element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat element4 = getStructuringElement(MORPH_RECT, Size(4, 4));
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
	//cvNamedWindow("二值化", WINDOW_NORMAL);
	//cvNamedWindow("腐蚀和膨胀", WINDOW_NORMAL);
	//imshow("二值化", binary);
	//imshow("腐蚀和膨胀", dilate3);
	//imwrite("D:/111/1threshold.jpg", binary);
	//imwrite("D:/111/2腐蚀和膨胀.jpg", dilate3);

	//3.寻找ROI
	vector<RotatedRect> rects;
	//3.1.查找轮廓
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilate3, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//3.2.筛选那些面积过小和过大的
	cout << "Num of contours: " << contours.size() << endl;
	vector<vector<Point>> contours_size8000;

	for (int i = 0; i < contours.size(); i++)
	{
		//计算当前轮廓的面积
		double area = contourArea(contours[i]);
		cout << i << "The area is: " << area << endl;

		//面积小于1000和大于600000的全部筛选掉
		if (area < 1000 || area > 600000)
			continue;
		else
		{
			contours_size8000.push_back(contours[i]);
		}
	}
	//cout << "size:" << contours_size8000.size() << endl;

	//4.填充掩膜区域
	Mat ContoursMast(dilate3.size(), CV_8U, Scalar(0));
	drawContours(ContoursMast, contours_size8000, -1, Scalar(255), CV_FILLED);
	//4.1取交集+二值化+膨胀
	Mat RoiImg, ROIImg1, ROIImg2;
	bitwise_and(ContoursMast, inputImage, RoiImg);
	threshold(RoiImg, ROIImg1, 200, 255, THRESH_BINARY);
	dilate(ROIImg1, ROIImg2, element3);

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
	imshow("ROIImg2", ROIImg2);
	//imwrite("D:/111/ROIImg2.jpg", ROIImg2);

	//waitKey();
	return ROIImg2;
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
void onChangeTrackBarErode(int pos, void* data)
{
	// 强制类型转换
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat eerode;
	Mat element = getStructuringElement(MORPH_RECT, Size(pos, pos));
	erode(srcImage, eerode, element);
	cv::imshow("Erode", eerode);
}


int scratch::scratchCheck(cv::Mat image)
{
	//int index = 0;
	//char str0[3];
	//sprintf(str0, "%03d", index);
	//cout << str0 << endl;

	//char strPath[200];
	//char str1[] = "D:/";
	//char str2[] = ".jpg";
	//sprintf(strPath, "%s%s%s", str1, str0, str2);
	//cout << strPath << endl;
	//cv::Mat image;
	//image = imread(strPath);//读图

	cv::Mat image2, canny, Mask, image666;
	imshow("Image", image);
	imwrite("D://660image.jpg", image);
	image.copyTo(image2);
	image.copyTo(image666);
	//cvtColor(image, image2, CV_RGB2GRAY, 0);
	//cvtColor(image, image666, CV_RGB2GRAY, 0);

	cv::Mat adpModel;
	adpModel = silkMask(image2);

	////自适应难以一步登天
	//adaptiveThreshold(image2, adpModel, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);
	//Mat element00 = getStructuringElement(MORPH_RECT, Size(3, 3));
	//erode(adpModel, adpModel, element00);
	//namedWindow("Erode");
	//int valueErode = 0;
	//createTrackbar("pos", "Erode", &valueErode, 30, onChangeTrackBarErode, &adpModel);
	//waitKey();

	imshow("adpModel", adpModel);
	imwrite("D://661model.jpg", adpModel);
	waitKey();

	/*
	namedWindow("Canny");
	imshow("Canny", image2);
	int valueCanny = 0;
	createTrackbar("pos", "Canny", &valueCanny, 300, onChangeTrackBarCanny, &image2);
	waitKey();
	*/
	Canny(image2, canny, 30, 77);
	//imshow("canny",canny);
	for (int j = 0; j<image2.rows; j++)
	{
		uchar* data = image2.ptr<uchar>(j);
		for (int i = 0; i<image2.cols; i++)
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
	adaptiveThreshold(image2, Mask, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 6 * 2 + 1, 8);
	imshow("Mask", Mask);

	for (int j = 0; j<image2.rows; j++)
	{
		uchar* data1 = Mask.ptr<uchar>(j);
		uchar* data2 = canny.ptr<uchar>(j);
		for (int i = 0; i<image2.cols; i++)
		{
			if (data1[i] + data2[i] == 0)
				data1[i] = 0;
			else
				data1[i] = 255;
		}
	}
	imshow("Mask2", Mask);
	imwrite("D://662preHandle.jpg", Mask);


	for (int j = 0; j<image2.rows; j++)
	{
		uchar* data3 = Mask.ptr<uchar>(j);
		uchar* data0 = adpModel.ptr<uchar>(j);
		for (int i = 0; i<image2.cols; i++)
		{
			if (data0[i] == 0)
				data3[i] = data3[i];
			else
				data3[i] = 0;
		}
	}
	imshow("Cut the printing", Mask);
	imwrite("D://663cutModel.jpg", Mask);

	Mat element11 = getStructuringElement(MORPH_RECT, Size(2, 2));
	erode(Mask, Mask, element11);
	dilate(Mask, Mask, element11);
	imshow("MaskErode", Mask);

	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(8, 8));
	erode(Mask, Mask, element33);
	dilate(Mask, Mask, element44);
	imshow("Final", Mask);
	imwrite("D://664defect.jpg", Mask);

	//Find the damages
	vector<vector<Point> > contours;//定义轮廓
	vector<Vec4i> hierarchy;//各个轮廓的继承关系
							//findContours函数寻找轮廓
	findContours(Mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(Mask.size(), CV_8U);
	int j = 0;
	//计算轮廓面积
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;    //零阶矩即为二值图像的面积  double area = moms.m00;  
								   //如果面积超出了设定的范围，则不再考虑该斑点  
		if (area > 80 && area < 10000)
		{
			drawContours(image666, contours, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
			j = j + 1;
		}
	}
	//标注与显示
	char t[256];
	snprintf(t, sizeof(t), "%01d", j);
	//sprintf_s(t, "%01d", j);
	string s = t;
	string txt = "NG number : " + s;
	putText(image666, txt, Point(20, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);

	imshow("drawing image", image666);
	imwrite("D:/665final.jpg", image666);

	/*char strPathout[200];
	char str1out[] = "../2D/Al/";
	char str2out[] = ".jpg";
	sprintf(strPathout,"%s%s%s",str1out,str0,str2out);
	imwrite(strPathout,image666);*/

	waitKey(0);

	int errorId = 0;
	return errorId;
}
