//Scratch detection (No Chinese)
#include "scratchwhite.hpp"

using namespace std;
using namespace cv;

void scratchwhite::findModel(cv::Mat depthImage, string path, Point* matchLocation, float threshold)
{
	Mat g_srcImage, g_templateImage, g_resultImage;
	g_templateImage = imread(path, 0);
	depthImage.copyTo(g_srcImage);

	//a) init a matrix to output result
	int resultImage_cols = g_srcImage.cols - g_templateImage.cols + 1;
	int resultImage_rows = g_srcImage.rows - g_templateImage.rows + 1;
	g_resultImage.create(resultImage_cols, resultImage_rows, CV_32FC1);

	//b) to match
	int g_nMatchMethod = cv::TM_CCOEFF_NORMED;//TM_SQDIFF,TM_SQDIFF_NORMED,TM_CCORR,TM_CCORR_NORMED,TM_CCOEFF,TM_CCOEFF_NORMED
	matchTemplate(g_srcImage, g_templateImage, g_resultImage, g_nMatchMethod);

	//c) to locate the best location
	double minValue; double maxValue; Point minLocation; Point maxLocation;

	minMaxLoc(g_resultImage, &minValue, &maxValue, &minLocation, &maxLocation, Mat());
	//d) for SQDIFF and SQDIFF_NORMED, the smaller the better, the other is inverse  
	if (g_nMatchMethod == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED)
	{
		matchLocation[0] = minLocation;
		//cout<<"min: "<<minValue<<endl;
	}
	else
	{
		matchLocation[0] = maxLocation;
		//cout<<"max: "<<maxValue<<endl;
	}
	matchLocation[1].x = matchLocation[0].x + g_templateImage.cols;
	matchLocation[1].y = matchLocation[0].y + g_templateImage.rows;

	//e) draw a rectangle to show result
	if (g_nMatchMethod == CV_TM_SQDIFF || g_nMatchMethod == CV_TM_SQDIFF_NORMED)
	{
		if (minValue<threshold)
			rectangle(g_srcImage, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), 2, 8, 0);
		else
		{
			matchLocation[0].x = 0; matchLocation[1].x = 0;
			matchLocation[0].y = 0; matchLocation[1].y = 0;
		}
	}
	if (g_nMatchMethod == CV_TM_CCORR || g_nMatchMethod == CV_TM_CCORR_NORMED || \
		g_nMatchMethod == CV_TM_CCOEFF || g_nMatchMethod == CV_TM_CCOEFF_NORMED)
	{
		if (maxValue>threshold)
			rectangle(g_srcImage, matchLocation[0], matchLocation[1], Scalar(0, 0, 255), 2, 8, 0);
		else
		{
			matchLocation[0].x = 0; matchLocation[1].x = 0;
			matchLocation[0].y = 0; matchLocation[1].y = 0;
		}
	}
}

cv::Mat scratchwhite::drawDetectLines(Mat image, const vector<Vec4i> lines, Scalar color)//draw the detected lines
{
	vector<Vec4i>::const_iterator it = lines.begin();
	while (it != lines.end())
	{
		Point pt1((*it)[0], (*it)[1]);
		Point pt2((*it)[2], (*it)[3]);
		line(image, pt1, pt2, color, 2); //  set the width of the line to 2
		++it;
	}
	return image;
}

void onChangeTrackBar1(int pos, void* data)
{
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat dstImage;
	//cv::threshold(srcImage, dstImage, pos, 255, 0);
	adaptiveThreshold(srcImage, dstImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, pos * 2 + 1, 8);
	cv::imshow("dyn_threshold", dstImage);
}
void onChangeTrackBarCanny1(int pos, void* data)
{
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat canny;
	cv::Canny(srcImage, canny, pos, 77);
	cv::imshow("Canny", canny);
}


int scratchwhite::whitescratchCheck(int batteryKind, cv::Mat image)
{
	/*if (argc != 3)
	{
		cout << "usage:./Scratch Batterykind(1 for A8,2 for A8P) No.picture" << endl;
		exit(0);
	}
	int batteryKind = atoi(argv[1]);*/

	if (batteryKind != 11)
	{
		cout << "No this kind of battery!" << endl;
		getchar();
		exit(0);
	}
	cout << "Battery kind: " << batteryKind << " (1 for A8,2 for A8F)" << endl;
	
	/*char strPath[200];
	char str1[] = "../2Dtemp/";
	char str2[] = ".jpg";
	sprintf(strPath, "%s%s%s", str1, argv[2], str2);
	cout << strPath << endl;
	cv::Mat image = imread(strPath);//读图*/

	cv::Mat image2, canny, Mask, image666;
	imshow("Image", image);
	//cvtColor(image, image2, CV_RGB2GRAY, 0);
	//cvtColor(image, image666, CV_RGB2GRAY, 0);
	image.copyTo(image2);
	image.copyTo(image666);
	
	/*
	namedWindow("Canny");
	imshow("Canny", image2);
	int valueCanny = 0;
	createTrackbar("pos", "Canny", &valueCanny, 300, onChangeTrackBarCanny1, &image2);
	cv::waitKey(0);
	*/
	Canny(image2, canny, 120, 77);
	//imshow("canny",canny);
	/*
	for (int j = 0; j<image2.rows; j++)
	{
		uchar* data = image2.ptr<uchar>(j);
		for (int i = 0; i<image2.cols; i++)
		{
			data[i] = 255 - data[i];
		}
	}
	*/
	/*
	namedWindow("dyn_threshold");
	imshow("dyn_threshold", image2);
	int value = 0;
	createTrackbar("pos", "dyn_threshold", &value, 255, onChangeTrackBar1, &image2);
	cv::waitKey(0);
	*/
	adaptiveThreshold(image2, Mask, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 6 * 2 + 1, 8);
	//imshow("Mask",Mask);
	
	//Combine the canny result and the adaptivethreshold result
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
	//imshow("Mask2",Mask);
	Mat element11 = getStructuringElement(MORPH_RECT, Size(2, 2));
	//erode(Mask, Mask, element11);
	dilate(Mask, Mask, element11);
	//imshow("MaskErode",Mask);



	/*
	Mat element55 = getStructuringElement(MORPH_RECT, Size(4, 4));
	Mat element66 = getStructuringElement(MORPH_RECT, Size(9, 9));
	Mat cut;
	erode(Mask,cut, element55);
	dilate(cut,cut, element66);
	for (int j=0; j<cut.rows; j++)
	{
	uchar* dataCut= cut.ptr<uchar>(j);
	for (int i=0; i<cut.cols; i++)
	{
	dataCut[i]=255-dataCut[i];
	}
	}
	Canny(cut,cut,100,1000);
	imshow("Cut",cut);
	// 检测直线
	vector<Vec4i> lines;
	HoughLinesP(cut,lines,1,CV_PI/180,200,50,20);
	image=drawDetectLines(image,lines,Scalar(0,255,0));
	imshow("Image",image);
	*/

	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(8, 8));
	erode(Mask, Mask, element33);
	dilate(Mask, Mask, element44);
	//imshow("Final3",Mask);

	/*//Fit the model of the battery area
	string pathBa = "../modelBa.jpg";
	Mat modelBa = imread(pathBa);
	cvtColor(modelBa, modelBa, CV_RGB2GRAY, 0);
	imshow("ModelBa",modelBa);
	//Mat element22 = getStructuringElement(MORPH_RECT, Size(7, 7));
	//dilate(model,model, element22);
	//imshow("Model2",model);
	cout<<"ModelBa size: "<<modelBa.cols<<"*"<<modelBa.rows<<endl;
	Point matchLocationBa[2];
	float modelThresholdBa = 0.2;
	findModel(image2,pathBa,matchLocationBa,modelThresholdBa);
	cout<<"ModelBa location: "<<matchLocationBa[0]<<" and "<<matchLocationBa[1]<<endl;
	for (int j=0; j<Mask.rows; j++)
	{
	uchar* dataMaskBa= Mask.ptr<uchar>(j);
	for (int i=0; i<Mask.cols; i++)
	{
	if(j<matchLocationBa[0].x || j>matchLocationBa[0].x+modelBa.cols || i<matchLocationBa[0].y || i>matchLocationBa[0].y+modelBa.rows )
	dataMaskBa[i]=0;
	}
	}*/

	//Find the damages
	vector<vector<Point> > contours;//
	vector<Vec4i> hierarchy;//findContours
	findContours(Mask, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	Mat drawing = Mat::zeros(Mask.size(), CV_8U);
	int scraNum = 0, longScraNum = 0;
	//calculate the S
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;    //this is the S
		if (area > 250 && area < 10000)
		{
			drawContours(image666, contours, i, Scalar(255), FILLED, 8, hierarchy, 0, Point());
			scraNum = scraNum + 1;
			if (area > 300)
				longScraNum = longScraNum + 1;
		}
	}
	//sign and show
	char t[256];
	snprintf(t, sizeof(t), "%01d", scraNum);
	//sprintf_s(t, "%01d", j);
	string s = t;
	string txt = "NG number : " + s;
	putText(image666, txt, Point(20, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 2, 8);
	
	imshow("drawing image", image666);
	cv::waitKey(0);

	int errorId = 0;
	float scraScore = longScraNum*0.4 + (scraNum - longScraNum)*0.1;
	if (scraScore > 1)
	{
		cout << "Warning: Too many scratchs or white points!" << endl;
		errorId = 1;
	}
	else
	{
		cout << "Scratch Check Pass!" << endl;
		errorId = 0;
	}
	return errorId;
}
