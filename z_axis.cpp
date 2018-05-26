//control 2d light
#include "z_axis.h"
#include <iostream>

using namespace std;

bool Z_AXIS::openCOM(LPCWSTR COMstr)
{
	hCom = CreateFile(COMstr,//��ͨ������TEXT("com4")
		GENERIC_READ | GENERIC_WRITE, //�����
		0, //ָ���������ԣ����ڴ��ڲ��ܹ������Ըò�������Ϊ0
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���

		FILE_ATTRIBUTE_NORMAL, //������������ֵΪFILE_FLAG_OVERLAPPED����ʾʹ���첽I/O���ò���Ϊ0����ʾͬ��I/O����
		NULL);

	if (hCom == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hCom);
		return FALSE;
	}
	else
		return TRUE;
	SetupComm(hCom, 1024, 1024); //���뻺����������������Ĵ�С����1024

								 /*********************************��ʱ����**************************************/
	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ
	TimeOuts.ReadIntervalTimeout = MAXDWORD;//�������ʱ
	TimeOuts.ReadTotalTimeoutMultiplier = 0;//��ʱ��ϵ��
	TimeOuts.ReadTotalTimeoutConstant = 0;//��ʱ�䳣��
										  //�趨д��ʱ
	TimeOuts.WriteTotalTimeoutMultiplier = 1;//дʱ��ϵ��
	TimeOuts.WriteTotalTimeoutConstant = 1;//дʱ�䳣��
	SetCommTimeouts(hCom, &TimeOuts); //���ó�ʱ

									  /*****************************************���ô���***************************/
	DCB dcb;
	GetCommState(hCom, &dcb);
	dcb.BaudRate = 9600; //������Ϊ
	dcb.ByteSize = 7; //ÿ���ֽ���7λ
	dcb.Parity = EVENPARITY; //����żУ��λ
	dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ
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

