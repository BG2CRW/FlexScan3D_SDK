//凹凸点监测
#include "aotupoint.hpp"

using namespace std;
using namespace cv;

vector<cv::Point2d> aotupoint::aotupointCheck(cv::Mat image)//test 8_HDR 10_HDR 11_HDR
{
	cv::Mat filterImage;
	cout<<"Aotupoint Check: This function is being designed."<<endl;
	int index=10;
	boxFilter(image, filterImage, -1, Size(20,20));
	cv::imshow("aotu",filterImage);
	cv::waitKey();
	vector<cv::Point2d> errorPosition(index);
	return errorPosition;
}
