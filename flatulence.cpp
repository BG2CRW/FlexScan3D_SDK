//flatulence check
#include "flatulence.hpp"
#define DEBUG 1

using namespace std;
using namespace cv;

int flatulence::flatulenceCheck(cv::Mat depthImage,float mean,float flaTh,int whiteTh,int whiteNum)
{
	int id=0;
	int soWhite=0;
	int nr=depthImage.rows;
	int nc=depthImage.cols;
	float sum=0;
	float num=0;
	for (int j=0; j<nr; j++) 
	{  
		uchar* data= depthImage.ptr<uchar>(j);  
		for (int i=0; i<nc; i++) 
		{  
			if(data[i]>whiteTh)
			{
				soWhite++;
			}
			if(data[i]>1)
			{
				sum+=(mean-(float)data[i])*(mean-(float)data[i]);
				num++;
			}
		}                    
	}
	cout<<"Sum: "<<sum<<",num: "<<num<<"White points: "<<soWhite<<endl;
	stdDevValue=100*sqrt(sum)/num;
	
#if DEBUG
	cout<<"StdDev: "<<stdDevValue<<endl;
#endif
	if(stdDevValue>flaTh)
	{
		id=1;
		cout<<"Warning: Flatulence!"<<endl;
	}
	else
	{
		if(soWhite>whiteNum)
		{
			id=2;
			cout<<"Warning: White points!"<<endl;
		}
		else
		{
			id=0;
			cout<<"Flatulence Check Pass!"<<endl;	
		}	
	}
	return id;
}
