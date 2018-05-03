//  基于OpenCV和Socket的图像传输（发送） 

#include "stdafx.h"
#include "SocketMatTransmissionClient.h"  
#include <iostream>
#include "stdio.h"
#include <stdlib.h>
#include <string.h>
using namespace std;
char buf[BUFFER_SIZE_LIMIT];
SocketMatTransmissionClient::SocketMatTransmissionClient(void)  
{  
}  


SocketMatTransmissionClient::~SocketMatTransmissionClient(void)  
{  
}  


int SocketMatTransmissionClient::socketConnect(const char* IP, int PORT)  
{  
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKADDR_IN  servaddr;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		//printf("create socket error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}  

	memset(&servaddr, 0, sizeof(servaddr));  
	servaddr.sin_addr.S_un.S_addr = inet_addr(IP);
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);  

	if (connect(sock, (sockaddr*)&servaddr, sizeof(servaddr)) < 0)   
	{
//		printf("connect error: %s(errno: %d)\n", strerror(errno), errno);  
		return -1;  
	}  
	else   
	{  
		printf("connect successful!\n");  
		return 1;
	}  
}  


void SocketMatTransmissionClient::socketDisconnect(void)  
{  
	closesocket(sock);  
	WSACleanup();
}  

int SocketMatTransmissionClient::transmit(cv::Mat image,int signal)  
{
	int pkg_num =(image.rows*image.cols/BUFFER_SIZE_LIMIT+1)*image.channels();
	//cout << image.rows << " " << image.cols << " " << image.channels() << " " << pkg_num << endl;
	data.rows = image.rows;
	data.cols = image.cols;
	data.channel = image.channels();
    data.signal=signal;
	data.pkg_size = pkg_num;
	int small_pkg = image.rows*image.cols / BUFFER_SIZE_LIMIT + 1;
	if (send(sock, (char *)(&data), sizeof(data), 0) < 0)
	{
		return -1;
	}
    if (image.empty())  
    {  
        printf("empty image\n");  
        return -1;  
    }  
	for (int k = 0; k < data.channel; k++)
	{
		for (int j = 0; j < small_pkg; j++)
		{
			for (int h = 0; h < image.rows; h++)
			{
				for (int w = 0; w < image.cols; w++)
				{
					if (data.channel == 3)
					{
						buf[0] = image.at<Vec3b>(h, w)[k];
					}
					else
					{
						buf[h*image.cols+w] = image.at<uchar>(h, w);
					}
				}
				
			}
			if (send(sock, buf, sizeof(buf), 0) < 0)
			{
				return -1;
			}
		}
	}
	return 0;
}  


cv::Mat SocketMatTransmissionClient::get()  
{
	
    if (recv(sock, (char*)&data_r,sizeof(data_r) , 0) < 0)  
    {  
        printf("paramater receive Failed!\n");  
    }
	cout << data_r.rows << " " << data_r.cols << " " << data_r.channel << endl;
	cv::Mat getImage(data_r.rows, data_r.cols, CV_8UC1, Scalar(0, 255, 255));
	recv(sock, (char*)buf, sizeof(buf), 0);
	for (int h = 0; h < getImage.rows; h++)
	{
		for (int w = 0; w < getImage.cols; w++)
		{
			getImage.at<uchar>(h, w)=buf[h*getImage.cols + w]  ;
		}
	}
    return getImage;  
}
