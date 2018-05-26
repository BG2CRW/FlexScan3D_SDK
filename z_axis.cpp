//control 2d light
#include "z_axis.h"
#include <iostream>

using namespace std;

bool Z_AXIS::openCOM(LPCWSTR COMstr)
{
	hCom = CreateFile(COMstr,//普通串口则TEXT("com4")
		GENERIC_READ | GENERIC_WRITE, //允许读
		0, //指定共享属性，由于串口不能共享，所以该参数必须为0
		NULL,
		OPEN_EXISTING, //打开而不是创建

		FILE_ATTRIBUTE_NORMAL, //属性描述，该值为FILE_FLAG_OVERLAPPED，表示使用异步I/O，该参数为0，表示同步I/O操作
		NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
		return FALSE;
	}
	else
		return TRUE;
	SetupComm(hCom, 1024, 1024); //输入缓冲区和输出缓冲区的大小都是1024

								 /*********************************超时设置**************************************/
	COMMTIMEOUTS TimeOuts;
	//设定读超时
	TimeOuts.ReadIntervalTimeout = MAXDWORD;//读间隔超时
	TimeOuts.ReadTotalTimeoutMultiplier = 0;//读时间系数
	TimeOuts.ReadTotalTimeoutConstant = 0;//读时间常量
										  //设定写超时
	TimeOuts.WriteTotalTimeoutMultiplier = 1;//写时间系数
	TimeOuts.WriteTotalTimeoutConstant = 1;//写时间常量
	SetCommTimeouts(hCom, &TimeOuts); //设置超时

									  /*****************************************配置串口***************************/
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //波特率为
	dcb.ByteSize = 7; //每个字节有7位
	dcb.Parity = EVENPARITY; //无奇偶校验位
	dcb.StopBits = ONESTOPBIT; //一个停止位
	SetCommState(hCom, &dcb);
}

bool Z_AXIS::closeCOM(LPCWSTR COMstr)
{
	return CloseHandle(hCom);
}

bool Z_AXIS::reset_z_axis()
{
	int i = 0;
	while (write(RESET, sizeof(RESET)) != 1)
	{
		i++;
		if (i == 30)
			return FALSE;
	}
	return TRUE;
}

bool Z_AXIS::move_distance(int distance)
{
	for (int i = 0; i < distance; i++)
	{
		int j = 0;
		while (write(UP_STEP, sizeof(UP_STEP)) != 1)
		{
			j++;
			if (j == 30)
				return FALSE;
		}
		int k = 0;
		while (write(SET_ZERO, sizeof(SET_ZERO)) != 1)
		{
			k++;
			if (k == 30)
				return FALSE;
		}

	}
	return TRUE;
}
int Z_AXIS::write(unsigned char* writebuf,int size)
{
	LPDWORD length = 0;
	if (WriteFile(hCom, writebuf, size, length, NULL) == FALSE)
	{
		printf("写入串口数据失败\n");
	}
	if (read() == 0x06)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
int Z_AXIS::read() 
{
	char buffer[1];
	DWORD read;
	ReadFile(hCom, buffer, sizeof(buffer), &read, NULL);
	return buffer[0];
}

