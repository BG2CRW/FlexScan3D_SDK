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

int Z_AXIS::reset_z_axis()
{

}

int Z_AXIS::move_distance(int distance)
{

}
int Z_AXIS::write(char* writebuf)
{

}
char* Z_AXIS::read() 
{

}

