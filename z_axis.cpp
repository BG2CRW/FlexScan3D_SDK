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
		printf("д�봮������ʧ��\n");
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

