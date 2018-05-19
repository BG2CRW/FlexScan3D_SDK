//Scratch detection (No Chinese)
#include "detect2d.hpp"

using namespace std;
using namespace cv;


void onChangeTrackBar(int pos, void* data);
void onChangeTrackBarCanny(int pos, void* data);

cv::Mat imageTemp;

string detect2d::scratchCheck(cv::Mat image, cv::Mat& silkModel2d, vector<vector<Point>>& contoursAl, vector<vector<Point>>& contoursLiquid)
{
	//cvtColor(image, image, CV_RGB2GRAY, 0);
	batteryKind = 2;
	if (batteryKind == 1)//Iphone8 Battery
	{
		//For test
		minRow = 150;
		maxRow = 1050;
		minCol = 50;
		maxCol = 1460;
	}
	if (batteryKind == 2)//Iphone8+ Battery
	{
		minRow = 210;
		maxRow = 1000;
		minCol = 270;
		maxCol = 1460;
	}
	if (batteryKind == 3)//HW Battery
	{
		minRow = 210;
		maxRow = 1000;
		minCol = 270;
		maxCol = 1460;
	}
	cv::Mat image2, imageEdge, edgeMask, Mask, imageBlack, imageDirty;
	imshow("Image", image);
	imwrite("D:/660image.jpg", image);
	image.copyTo(image2);
	image.copyTo(imageEdge);
	image.copyTo(imageBlack);
	image.copyTo(imageTemp);
	image.copyTo(imageDirty);

	//get the edge mask and the edge area(imageEdge)
	edgeMask = edgeMake(imageEdge);

	//Detect the EDGE (mainly for hole point)
	string edgeID = edgeDetect(imageEdge, edgeMask);
	cout << "Result ID for Edge detection: " << edgeID << ". (1 for OK,2 for NG)" << endl;

	//detect the BLACK defects
	string blackID = blackDetect(imageBlack, edgeMask);
	cout << "Result ID for Black detection: " << blackID << ". (1 for OK,2 for NG)" << endl;
	imshow("Black", imageBlack);

	//make adaptive model
	cv::Mat adpModel, adpROI;
	////make model ROI using adaptive method
	//bitwise_not(image2, adpROI);
	//adaptiveThreshold(adpROI, adpROI, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);
	//Mat elementAdp = getStructuringElement(MORPH_RECT, Size(6, 6));
	//dilate(adpROI, adpROI, elementAdp);
	////imshow("One step adp", adpROI);
	////imwrite("D:/661modeladp.jpg", adpROI);
	//Model Make
	adpModel = silkMask(image2, edgeMask, adpROI);
	imshow("adpModel0", imageTemp);
	imwrite("D:/661model0.jpg", imageTemp);
	imshow("adpModel", adpModel);
	imwrite("D:/661model.jpg", adpModel);
	adpModel.copyTo(silkModel2d);
	//waitKey();

	//Prehandle of the image
	Mask = preProcess(image2);

	//Cut the silk model
	bitwise_or(imageBlack, adpModel, adpModel);
	bitwise_not(adpModel, adpModel);
	bitwise_and(Mask, adpModel, Mask);
	imshow("Cut the silk", Mask);
	imwrite("D:/663cutModel.jpg", Mask);

	//Cut the edge
	bitwise_and(Mask, edgeMask, Mask);
	//edgeCut(Mask);
	imshow("Cut the edge", Mask);
	imwrite("D:/664cutEdge.jpg", Mask);

	//Outstand the defect
	Mat element11 = getStructuringElement(MORPH_RECT, Size(2, 2));
	erode(Mask, Mask, element11);
	dilate(Mask, Mask, element11);
	imwrite("D:/665defect.jpg", Mask);

	//Show all defects
	cv::Mat imageShow, maskShow;
	image.copyTo(imageShow);
	Mask.copyTo(maskShow);
	showDefect(imageShow, maskShow);

	//Detect the LIQUID
	cv::Mat imageLiquid, maskLiquid;
	image.copyTo(imageLiquid);
	Mask.copyTo(maskLiquid);
	string liquidID = liquidDetect(imageLiquid, maskLiquid, contoursLiquid);
	cout << "Result ID for Liquid detection: " << liquidID << ". (1 for OK,2 for NG)" << endl;

	//Detect the AL
	cv::Mat imageAl, maskAl;
	image.copyTo(imageAl);
	Mask.copyTo(maskAl);
	string alID = alDetect(imageAl, maskAl, contoursAl);
	cout << "Result ID for Al detection: " << alID << ". (1 for OK,2 for NG)" << endl;

	//Detect the SCRATCH
	cv::Mat imageScratch, maskScratch;
	image.copyTo(imageScratch);
	Mask.copyTo(maskScratch);
	string scratchID = scratchDetect(imageScratch, maskScratch);
	cout << "Result ID for Scratch detection: " << scratchID << ". (1 for OK,2 for NG)" << endl;

	
	//detect dirty
	Mat adpModelTemp;
	bitwise_and(adpModel, edgeMask, adpModelTemp);
	string dirtyID;
	dirtyID = dirtyDetect(adpModelTemp, imageScratch, imageDirty);
	cout << "Result ID for dirty detection: " << dirtyID << ". (1 for OK,2 for NG)" << endl;

	waitKey(0);

	string errorID = liquidID + alID + blackID + scratchID + edgeID + dirtyID;
	return errorID;
}

cv::Mat detect2d::edgeMake(cv::Mat origin)
{
	//1.Make the edge
	//1.1binary
	Mat binary;
	threshold(origin, binary, 250, 255, THRESH_BINARY_INV);
	//1.2Pre process
	Mat element009 = getStructuringElement(MORPH_RECT, Size(9, 9));
	Mat element007 = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(binary, binary, element009);
	dilate(binary, binary, element009);
	dilate(binary, binary, element007);
	erode(binary, binary, element007);
	//1.3get the outer outline
	vector<vector<Point>> contours, contoursEdge;
	vector<Vec4i> hierarchy;
	findContours(binary, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	//cout << "outline number: " << contours.size() << endl;
	for (int i = 0; i < contours.size(); ++i)
	{
		if (contourArea(contours[i]) > 500000)
			contoursEdge.push_back(contours[i]);
	}
	//1.4draw the outer outline and get the outer edge
	Mat edgeMask(binary.size(), CV_8U, Scalar(0));
	drawContours(edgeMask, contoursEdge, -1, Scalar(255), FILLED);
	Mat element301 = getStructuringElement(MORPH_RECT, Size(301, 301));
	erode(edgeMask, edgeMask, element301);
	dilate(edgeMask, edgeMask, element301);
	//imshow("edge mask", edgeMask);
	//imwrite("D:/VS_Project/Image_Test/edgeHandle/edgetest/mask1.jpg", mask1);

	//2.get the edge model
	Mat innerEdge, innerMask;
	Mat element039 = getStructuringElement(MORPH_RECT, Size(39, 39));
	Mat element055 = getStructuringElement(MORPH_RECT, Size(55, 55));
	erode(edgeMask, innerEdge, element039);
	erode(edgeMask, innerMask, element055);
	//bitwise_not(innerEdge, innerEdge);
	bitwise_not(innerMask, innerMask);
	//imshow("annular mask", innerEdge);
	////Show the edge cut effect
	//Mat showEffect;
	//origin.copyTo(showEffect);
	//bitwise_and(showEffect, innerEdge, showEffect);
	//imshow("Show Effect", showEffect);

	//3.get the annular edge
	bitwise_and(origin, innerMask, origin);
	bitwise_and(origin, edgeMask, origin);
	//imshow("Annular Edge", origin);

	//4.reverse the model
	//bitwise_not(innerEdge, innerEdge);

	for (int j = 0; j<innerEdge.rows; j++)
	{
		uchar* data1 = innerEdge.ptr<uchar>(j);
		uchar* data2 = origin.ptr<uchar>(j);
		for (int i = 0; i<innerEdge.cols; i++)
		{
			if (i>maxCol)
			{
				data1[i] = 0;
				data2[i] = 0;
			}
		}
	}

	return innerEdge;
}

string detect2d::edgeDetect(cv::Mat inputImage, cv::Mat edgeMask)
{
	string resultID = "1";

	Mat imageEdge;
	inputImage.copyTo(imageEdge);

	//Using adpThreshold to find the Hole Point
	for (int j = 0; j<inputImage.rows; j++)
	{
		uchar* data1 = inputImage.ptr<uchar>(j);
		for (int i = 0; i<inputImage.cols; i++)
		{
			data1[i] = 255 - data1[i];
		}
	}
	/*namedWindow("dyn_threshold", CV_WINDOW_NORMAL);
	imshow("dyn_threshold", imageGray);
	int value = 0;
	createTrackbar("pos", "dyn_threshold", &value, 300, onChangeTrackBar, &imageGray);
	waitKey();*/
	Mat adpEdge;
	adaptiveThreshold(inputImage, adpEdge, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 5 * 2 + 1, 8);

	//Outstand the Hole Point
	Mat elementAo = getStructuringElement(MORPH_RECT, Size(2, 2));
	dilate(adpEdge, adpEdge, elementAo);
	//erode(adpEdge, adpEdge, elementAo);
	namedWindow("adpEdge", CV_WINDOW_NORMAL);
	imshow("adpEdge", adpEdge);

	//Cut the wrong hole exactly on the outer edge
	Mat outerEdgeMask;
	Mat element039 = getStructuringElement(MORPH_RECT, Size(39, 39));
	Mat elementOuter = getStructuringElement(MORPH_RECT, Size(5, 5));
	dilate(edgeMask, outerEdgeMask, element039);//Transform the mask into outerEdgeMask
	erode(outerEdgeMask, outerEdgeMask, elementOuter);
	bitwise_and(adpEdge, outerEdgeMask, adpEdge);
	/*namedWindow("adpEdge1", CV_WINDOW_NORMAL);
	imshow("adpEdge1", adpEdge);*/

	//Find and filter the Hole Point
	vector<vector<Point>> contoursEdge, contoursAo;
	vector<Vec4i> hierarchyEdge;
	findContours(adpEdge, contoursEdge, hierarchyEdge, CV_RETR_TREE, CV_CHAIN_APPROX_NONE);
	int smallNum = 0, bigNum = 0;
	for (int i = 0; i < contoursEdge.size(); i++)
	{
		double area = contourArea(contoursEdge[i]);
		double templength = arcLength(contoursEdge[i], true);
		if (hierarchyEdge[i][2] == -1 && hierarchyEdge[i][3] != -1)
		{
			if (area > 30 && area < 300)
			{
				double r = templength / (2 * 3.1415);
				if (3.1415 * r * r < 3 * area && templength < 150)
				{
					cout << "No." << i << ": Area is: " << area << ", Length is: " << templength << endl;
					cout << 3.1415*r*r / area << endl;
					contoursAo.push_back(contoursEdge[i]);

					//Score the hole 1mm==6.67pixels
					if (area > 100)
					{
						bigNum++;
					}
					else
					{
						smallNum++;
					}
				}
			}
		}
	}

	//Judge the score
	if (smallNum * 0.2 + bigNum * 0.5 > 1)
	{
		cout << "    EdgeHole Big(0.5):" << bigNum << ", small(0.2): " << smallNum << ". NG!" << endl;
		resultID = "2";
	}

	//Draw the results
	for (int i = 0; i < contoursAo.size(); i++)
	{
		drawContours(imageEdge, contoursAo, i, Scalar(255), FILLED, 8, hierarchyEdge, 0, Point());
	}
	namedWindow("AoPoint", CV_WINDOW_NORMAL);
	imshow("AoPoint", imageEdge);

	return resultID;
}

string detect2d::blackDetect(cv::Mat inputImage, cv::Mat edgeMask)
{
	string resultID = "1";
	//binary(not stable)
	threshold(inputImage, inputImage, 75, 255, THRESH_BINARY_INV);
	//cut edge using the model
	bitwise_and(inputImage, edgeMask, inputImage);
	//erode and dilate
	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(12, 12));
	erode(inputImage, inputImage, element1);
	dilate(inputImage, inputImage, element2);

	//Find the black
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	//compute the area
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;
		if (area > 5)
		{
			j = j + 1;
		}
	}
	if (j > 0)
	{
		cout << "    There are " << j << " black shine." << endl;
		resultID = "2";
	}

	return resultID;
}

cv::Mat detect2d::silkMask(cv::Mat inputImage, cv::Mat edgeMask, cv::Mat adpROI)
{
	//1.binary
	surfaceIndex = 1;
	Mat binary;
	//Mat bilateral;
	//bilateralFilter(inputImage, bilateral, 10, 20, 5);
	//imshow("bilateral filter", bilateral);
	threshold(inputImage, binary, 240, 255, THRESH_BINARY);
	//adaptiveThreshold(img, binary, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 1000 * 2 + 1, 8);
	//1.2.Set the ROI
	bitwise_and(binary, edgeMask, binary);
	//imshow("Binary", binary);

	//2.erode and dilate
	Mat dilate1, erode1, dilate2, erode2, dilate3;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(19, 19));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(19, 19));
	Mat element3 = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat element4 = getStructuringElement(MORPH_RECT, Size(3, 3));
	dilate(binary, dilate1, element2);
	erode(dilate1, erode1, element1);
	dilate(erode1, dilate2, element2);
	erode(dilate2, erode2, element1);

	dilate(erode2, dilate3, element3);

	////save and show
	//imshow("erode&dilate0", erode2);
	imshow("erode&dilate", dilate3);
	//imwrite("D:/111/2erode&dilate.jpg", dilate3);

	//3.choose right ROI
	//3.1.find contours
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(dilate3, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//3.2.delete the contours which is too big or too small
	//cout << "Num of contours: " << contours.size() << endl;
	vector<vector<Point>> contours_size8000;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);
		//cout << i << "The area is: " << area << endl;

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
		cout << "    This is Engilsh face!1111111111111111" << endl;
	}
	else
		cout << "    This is Chinese face!2222222222222222" << endl;
	//cout << "size:" << contours_size8000.size() << endl;
	Mat ContoursMast(dilate3.size(), CV_8U, Scalar(0));
	drawContours(ContoursMast, contours_size8000, -1, Scalar(255), CV_FILLED);


	//make the printing model violently
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

		//imshow("priting", printing);

		bitwise_or(ContoursMast, printing, ContoursMast);
	}


	////3.3.combined with adaptive threshold ROI
	//bitwise_and(ContoursMast, adpROI, ContoursMast);

	//vector<vector<Point>> contoursROI,contoursFinal;
	//vector<Vec4i> hierarchyROI;
	//findContours(ContoursMast, contoursROI, hierarchyROI, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//for (int i = 0; i < contoursROI.size(); i++)
	//{
	//	double area = contourArea(contoursROI[i]);
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
	//imshow("final ROI", finalROI);




	//4.get the silk model
	//4.1and+binary+dilate
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

	////4.2show and save
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

string detect2d::liquidDetect(cv::Mat origin, cv::Mat inputImage, vector<vector<Point>>& contoursLiquid)
{
	string resultID = "1";

	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(10, 10));
	erode(inputImage, inputImage, element33);
	dilate(inputImage, inputImage, element44);
	//imshow("liquid0", inputImage);

	//Find the damages
	vector<vector<Point>> contours;
	//vector<vector<Point>> contoursvalue; 
	vector<Vec4i> hierarchy;
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	//find the black and white points
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
			contoursLiquid.push_back(contours[i]);
		}
	}
	for (int i = 0; i < contoursLiquid.size(); i++)
	{
		drawContours(origin, contoursLiquid, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
	}
	if (contoursLiquid.size() > 0)
		resultID = "2";
	cout << "    There are " << liquidNum << " liquid defects." << endl;
	imshow("liquid", origin);
	//imwrite("F:/liquidResult.jpg", origin);

	return resultID;
}

string detect2d::alDetect(cv::Mat origin, cv::Mat inputImage, vector<vector<Point>>& contoursAl)
{
	string resultID = "2";
	Mat elementAl = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(inputImage, inputImage, elementAl);
	//dilate(InputImage, InputImage, elementAl);
	imshow("Al0", inputImage);
	//imwrite("F:/Al.jpg", inputImage);

	vector<vector<Point>> contours;
	//vector<vector<Point>> contoursfinal;
	vector<Vec4i> hierarchy;
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	//detect Al
	for (int Size = 0; Size < contours.size(); Size++)
	{
		Moments moms = moments(Mat(contours[Size]));
		double area = moms.m00;
		double value = 0;
		double meanValue1 = 0, meanValue2 = 0, meanValue = 0;

		if (area > 10)	//choose right area
		{
			//compute the mean of the contours
			for (int Size0 = 0; Size0 < contours[Size].size(); Size0++)
			{
				value += origin.at<uchar>(contours[Size][Size0].y, contours[Size][Size0].x);
			}
			meanValue1 = value / contours[Size].size();

			//compute the mean of contours area
			double sumAl = 0, numAl = 0;
			Mat alMean(origin.size(), CV_8U, Scalar(0));
			drawContours(alMean, contours, Size, Scalar(255), FILLED, 8, hierarchy, 0, Point());
			bitwise_and(alMean, origin, alMean);
			for (int j = 0; j<alMean.rows; j++)
			{
				uchar* data = alMean.ptr<uchar>(j);
				for (int i = 0; i<alMean.cols; i++)
				{
					if (data[i] > 0)
					{
						sumAl = sumAl + data[i];
						numAl++;
					}
				}
			}
			meanValue2 = sumAl / numAl;

			//compute the mean of the middle area of the contours
			meanValue = (sumAl - value) / (numAl - contours[Size].size());

			if (meanValue > 235)	//Set the value
			{
				cout << "    No." << Size + 1 << " Al: area: " << area << " mean: " << meanValue << endl;
				contoursAl.push_back(contours[Size]);
			}
		}
	}
	//sort(mean.begin(), mean.end(), greater<double>());
	//cout << "Max:" << mean[0] << endl;
	for (int i = 0; i < contoursAl.size(); i++)
	{
		drawContours(origin, contoursAl, i, Scalar(0), FILLED, 8, hierarchy, 0, Point());
	}
	imshow("Al", origin);
	//imwrite("F:/AlResult.jpg", origin);

	if (contoursAl.size() > 0)
		resultID = "2";

	return resultID;
}

string detect2d::scratchDetect(cv::Mat origin, cv::Mat inputImage)
{
	string resultID = "1";

	////Make a adaptive printing in Chinese face
	//Mat origin2, adpPrinting;
	//origin.copyTo(origin2);
	//for (int j = 0; j<origin2.rows; j++)
	//{
	//	uchar* data = origin2.ptr<uchar>(j);
	//	for (int i = 0; i<origin2.cols; i++)
	//	{
	//		data[i] = 255 - data[i];
	//	}
	//}
	//imwrite("F:/119Scratch.jpg", inputImage);
	//adaptiveThreshold(origin2, adpPrinting, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 6 * 2 + 1, 26);
	//Mat element22 = getStructuringElement(MORPH_RECT, Size(4, 4));
	//dilate(adpPrinting, adpPrinting, element22);
	//imwrite("F:/110adpPrinting.jpg", adpPrinting);
	//subtract(inputImage, adpPrinting, inputImage);
	//imwrite("F:/111Scratch.jpg", inputImage);

	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element88 = getStructuringElement(MORPH_RECT, Size(6, 6));
	Mat element55 = getStructuringElement(MORPH_RECT, Size(3, 3));
	erode(inputImage, inputImage, element33);
	//imshow("scratch000", inputImage);
	dilate(inputImage, inputImage, element88);
	//imshow("scratch00", inputImage);
	erode(inputImage, inputImage, element55);
	//imshow("scratch0", inputImage);
	//imwrite("F://Scratch.jpg", inputImage);

	//Find the damages
	vector<vector<Point>> contours;
	vector<vector<Point>> contoursvalue;
	vector<Vec4i> hierarchy;
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	//compute the area and perimeter
	vector<double> length;
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;	
		double templength = arcLength(contours[i], true);    //compute the perimeter

		if (area > 60 && area < 10000)	//choose right area
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
	//imwrite("F:/scratchResult.jpg", origin);

	//compute the real length
	double scale = 1600.0/116.5;
	int longScratch = 0, midScratch = 0, shortScratch = 0;
	for (int g = 0; g < length.size(); g++)
	{
		length[g] /= scale;
		if (length[g] / 2 > 1)
		{
			if (length[g] / 2 > 15)
			{
				if (length[g] / 2 > 30)
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
	if (longScratch * 1 + midScratch*0.166 + shortScratch*0.125 > 1)
	{
		cout << "    Scratch Long(1):" << longScratch << ", Mid(0.166): " << midScratch << ", Short(0.125): " << shortScratch << ". NG!" << endl;
		resultID = "2";
	}

	return resultID;
}

void detect2d::showDefect(cv::Mat finalShow, cv::Mat inputImage)
{
	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element44 = getStructuringElement(MORPH_RECT, Size(8, 8));
	erode(inputImage, inputImage, element33);
	dilate(inputImage, inputImage, element44);

	//Find the damages
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(inputImage, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int j = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		Moments moms = moments(Mat(contours[i]));
		double area = moms.m00;
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

	//if (2 > 1)//mark the edge
	//{
	//	for (int j = 0; j<finalShow.rows; j++)
	//	{
	//		uchar* datad = finalShow.ptr<uchar>(j);
	//		for (int i = 0; i<finalShow.cols; i++)
	//		{
	//			if (j == minRow || j == maxRow || i == minCol || i == maxCol)
	//			{
	//				datad[i] = 0;
	//			}
	//		}
	//	}
	//}

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

string detect2d::dirtyDetect(cv::Mat adpModel, cv::Mat imageScratch, cv::Mat imageDirty)
{
	string dirty_detection_flag = "1";
	Mat element33 = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat element55 = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat elementzz = getStructuringElement(MORPH_RECT, Size(35, 35));
	Mat imgDirty, imgDirtywhite, inputImage;
	imageDirty.copyTo(inputImage);
	bitwise_and(imageScratch, adpModel, imgDirty);
	threshold(imgDirty, imgDirtywhite, 210, 255, THRESH_BINARY);
	dilate(imgDirtywhite, imgDirtywhite, element55);
	Mat dirtyMask2;
	adpModel.copyTo(dirtyMask2);
	dilate(dirtyMask2, dirtyMask2, elementzz);
	erode(dirtyMask2, dirtyMask2, elementzz);
	erode(dirtyMask2, dirtyMask2, elementzz);


	vector<vector<Point>> contours00, contours11;
	findContours(imgDirtywhite, contours00, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	for (int i = 0;i < contours00.size();++i)
	{
		if ((contourArea(contours00[i]) > 40) && (contourArea(contours00[i])<50000))
		{
			contours11.push_back(contours00[i]);
		}
	}
	Mat whiteDirty;
	imageDirty.copyTo(whiteDirty);
	drawContours(whiteDirty, contours11, -1, Scalar(0));
	imwrite("D:/白色脏污结果.jpg", whiteDirty);

	//黑色脏污
	Mat blackbinary;
	threshold(imageDirty, blackbinary, 95, 255, THRESH_BINARY_INV);
	//adaptiveThreshold(imageDirty, blackbinary, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY_INV, 51, 40);
	//imwrite("D:/黑色脏污原模型.jpg", imageDirty);
	dilate(blackbinary, blackbinary, element55);
	erode(blackbinary, blackbinary, element55);
	//imwrite("D:/黑色脏污二值化.jpg", blackbinary);
	vector<vector<Point>> contours22, contours33;
	findContours(blackbinary, contours22, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);
	for (int i = 0;i < contours22.size();++i)
	{
		if ((contourArea(contours22[i]) > 300) && (contourArea(contours22[i]) < 5000))
		{
			contours33.push_back(contours22[i]);
			contours11.push_back(contours22[i]);
		}
	}
	Mat blackDirty;
	imageDirty.copyTo(blackDirty);
	drawContours(blackDirty, contours33, -1, Scalar(0));
	imwrite("D:/黑色脏污结果.jpg", blackDirty);


	Mat maskdirty2(blackDirty.size(), CV_8U, Scalar(0));
	drawContours(maskdirty2, contours11, -1, Scalar(255), CV_FILLED);
	for (int i = 0;i < blackDirty.rows;++i)
	{
		uchar *ptr1 = dirtyMask2.ptr<uchar>(i);
		uchar *ptr2 = maskdirty2.ptr<uchar>(i);
		for (int j = 0;j < blackDirty.cols;++j)
		{
			if (ptr1[j] == 0)
				ptr2[j] == 0;
		}
	}


	vector<vector<Point>> contours44;
	findContours(maskdirty2, contours44, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);

	float dirtymarks = 0;
	for (int i = 0;i < contours44.size();++i)
	{
		//cout << "轮廓面积" <<i<<"："<< contourArea(contours44[i]) << endl;
		float dirtyarea = contourArea(contours44[i]);
		if ((dirtyarea > 1800)&& (dirtyarea<80000))
			dirtymarks += 1;
		else if (dirtyarea > 900)
		{
			dirtymarks += 0.5;
		}
		else if (dirtyarea > 500)
		{
			dirtymarks += 0.34;
		}
		else if (dirtyarea > 200)
		{
			dirtymarks += 0.25;
		}
	}


	if (dirtymarks > 4)
		dirty_detection_flag = "2";
	cout << "轮廓44个数：" << contours44.size() << endl;
	cout << "脏污检测：" << dirtymarks << endl;
	if (surfaceIndex == 2)
		dirty_detection_flag = "1";

	return dirty_detection_flag;
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
	imshow("dyn_threshold", inputImage);
	int value = 0;
	createTrackbar("pos", "dyn_threshold", &value, 30, onChangeTrackBar, &inputImage);
	waitKey();
	*/
	adaptiveThreshold(inputImage, Mask, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 6 * 2 + 1, 8);
	//imshow("Mask", Mask);

	bitwise_or(Mask, canny, Mask);
	//imshow("Mask2", Mask);
	imwrite("D:/662preHandle.jpg", Mask);

	return Mask;
}

void pitsdetect( cv::Mat imgFCN, ::Mat img) {

	Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
	cv::Mat img_erosion_right, img_dilation_right, bi_thre_right, img_binary_right;
	erode(imgFCN, img_erosion_right, element1);
	dilate(img_erosion_right, img_dilation_right, element1);
	threshold(img_dilation_right, img_binary_right, 40, 255, THRESH_BINARY);

	//Find the damages
	vector<vector<Point>> contours;
	vector<vector<Point>> contoursvalue;
	vector<Vec4i> hierarchy;
	findContours(img_binary_right, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	vector<double> length;
	int numofpits = 0;
	for (int i = 0; i < contours.size(); i++)
	{
		double area = contourArea(contours[i]);

		if (area > 40)	//choose right area
		{
			Rect rect = boundingRect(contours[i]);//检测外轮廓  
			rectangle(img, rect, Scalar(0, 0, 0), 3);//对外轮廓加矩形框
			numofpits++;
		}
	}
	imshow("pits display", img);

	std::cout << "numbers of pits is:"<< numofpits << endl;
	if (numofpits > 3)
		std::cout << "pits NG!" << endl;
	else
		std::cout << "pits OK!" << endl;

}

void onChangeTrackBar(int pos, void* data)
{
	//Coercive type conversion
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat dstImage;
	//cv::threshold(srcImage, dstImage, pos, 255, 0);
	adaptiveThreshold(srcImage, dstImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, pos * 2 + 1, 8);
	cv::imshow("dyn_threshold", dstImage);
}
void onChangeTrackBarCanny(int pos, void* data)
{
	//Coercive type conversion
	cv::Mat srcImage = *(cv::Mat*)(data);
	cv::Mat canny;
	cv::Canny(srcImage, canny, pos, 77);
	cv::imshow("Canny", canny);
}
