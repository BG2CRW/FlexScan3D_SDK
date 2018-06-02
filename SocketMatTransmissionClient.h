//  基于OpenCV和Socket的图像传输（发送） 
#ifndef __SOCKETMATTRANSMISSIONCLIENT_H__  
#define __SOCKETMATTRANSMISSIONCLIENT_H__  
#include "stdafx.h"
#include "opencv2/opencv.hpp"  
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>
#include <Windows.h>   
#pragma comment (lib, "ws2_32.lib") //加载 ws2_32.dll
using namespace cv;

#define BUFFER_SIZE_LIMIT 19300000

struct sentbuf  
{ 
	unsigned int signal;
	unsigned int pkg_size;
	
	unsigned int channel;
	unsigned int rows;
	unsigned int cols;
	//int flag;
	
};

class SocketMatTransmissionClient  
{  
	public:  
	SocketMatTransmissionClient(void);  
	~SocketMatTransmissionClient(void);  

	private:  
	SOCKET sock;
	struct sentbuf data;  ///传播的数据组 
	struct sentbuf data_r;
	public:  

	// 打开socket连接  
	// params : IP      服务器的ip地址  
	//          PORT    传输端口  
	// return : -1      连接失败  
	//          1       连接成功  
	int socketConnect(const char* IP, int PORT, cv::Mat img, int open);


	// 传输图像  
	// params : image 待传输图像  
	// return : -1      传输失败  
	//          1       传输成功  
	int transmit(cv::Mat image,int signal);  
	cv::Mat get();

	// 断开socket连接  
	void socketDisconnect(void);  
};  

#endif  
