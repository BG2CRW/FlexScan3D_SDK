// TestAPI.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "ScanInterfaceC.h"
#include "Callbacks.h"
#include "detect2d.hpp"
#include "detect3d.hpp"
#include <iostream>  
#include <boost/timer.hpp>
#include "opencv2/opencv.hpp"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "opencv2\highgui.hpp"
#include "opencv\cv.h"
#include "opencv2\core.hpp"
#include <math.h>
#include "SocketMatTransmissionClient.h"
#include "z_axis.h"
#include <vector>


// Include files to use the PYLON API.
#include "pylon\PylonIncludes.h"
#include <pylon\PylonGUI.h>
#include "config.hpp"

int pic2Dnum = 5;
static const uint32_t c_countOfImagesToGrab = 1;
class detect2d Detect2d;
class detect3d Detect3d;
class Z_AXIS Motor;
class SocketMatTransmissionClient client;
cv::Mat imgdepthVert;
cv::Mat imgdepthHor;
cv::Mat imageBasler;
char path3D_prefix[]= "D:/vs2015_ws/ScanInterface/examples/c++/ReceiveVertices/src/network_3d/data/newSrc/";
char path2D_prefix[] = "D:/Data/2D/";
char path_suffix[] = ".jpg";
char path_3DHor_suffix[] = "_Hor.jpg";
char path_3DVert_suffix[] = "_Vert.jpg";
int counter = 0;
int errorReport;

using namespace std;
using namespace Pylon;
using namespace cv;
cv::Mat socket2D(vector<cv::Mat> img)
{
	if (-1 == client.socketConnect("127.0.0.1", 66))
	{
		std::cout << "connect failed" << endl;
	}

	int open = 0;//0:open,1:close
	for (int i = 0; i < 5; i++)
	{
		client.transmit(img[i], open);
		Sleep(200);
	}
	cv::Mat imageReceive = client.get();
	client.socketDisconnect();
	//cv::imshow("clientReceive", imageReceive);
	//cv::waitKey();
	return imageReceive;

}


cv::Mat socket3D(cv::Mat img)
{
	if (-1 == client.socketConnect("127.0.0.1", 55))
	{
		std::cout << "connect failed" << endl;
	}

	int open = 0;//0:open,1:close
	client.transmit(img, open);
	cv::Mat imageReceive = client.get();
	client.socketDisconnect();
	return imageReceive;

}

int main(int argc, char* argv[])
{
#ifdef GRAB
	if (Motor.openCOM(TEXT("COM7")) == FALSE)
	{
		std::cout << "Failed to open serial port" << endl;
		return 0;
	}
#endif
	while (1)
	{
		counter++;
		std::cout << "counter:" << counter << endl;
		char str0[5];
		sprintf(str0, "%05d", counter);
		char strPath2D[200], strPath3DVert[200], strPath3DHor[200];
		sprintf(strPath2D, "%s%s", path2D_prefix, str0);
		sprintf(strPath3DVert, "%s%s%s", path3D_prefix, str0, path_3DVert_suffix);
		sprintf(strPath3DHor, "%s%s%s", path3D_prefix, str0, path_3DHor_suffix);
		WCHAR dir_name[64];
		swprintf(dir_name, L"%S", strPath2D);

#ifdef GRAB
		// The exit code of the sample application.
		int exitCode = 0;
		if(Motor.reset_z_axis()==FALSE)
		{
			std::cout << "Z-axis reset failed!" << endl;
		}
		std::cout << "Please put a side of the next battery in basler!" << endl;
		getchar();
		bool flag = CreateDirectory(dir_name, NULL);
		for (int i = 0; i < pic2Dnum; i++)
		{
			char strFileName[200];
			sprintf(strFileName, "%s/%d%s", strPath2D, i, path_suffix);
			// Before using any pylon methods, the pylon runtime must be initialized. 
			PylonInitialize();
			try
			{
				CInstantCamera camera(CTlFactory::GetInstance().CreateFirstDevice());// Create an instant camera object with the camera device found first.
				//cout << "Using device " << camera.GetDeviceInfo().GetModelName() << endl;// Print the model name of the camera.
				camera.MaxNumBuffer = 5;
				camera.StartGrabbing(c_countOfImagesToGrab);// Start the grabbing of c_countOfImagesToGrab images.
				CGrabResultPtr ptrGrabResult;// This smart pointer will receive the grab result data.
				camera.IsGrabbing();
				camera.RetrieveResult(5000, ptrGrabResult, TimeoutHandling_ThrowException);// Wait for an image and then retrieve it. A timeout of 5000 ms is used.

				if (ptrGrabResult->GrabSucceeded())// Image grabbed successfully?
				{
					// Access the image data.
					std::cout << "SizeX: " << ptrGrabResult->GetWidth() << "SizeY: " << ptrGrabResult->GetHeight() << endl;
					const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
					//cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;

					imageBasler = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)ptrGrabResult->GetBuffer());
					cv::imwrite(strFileName, imageBasler);

					boost::timer t2;
					std::cout << "Time of scratch check: " << t2.elapsed() << endl;
				}
				else
				{
					std::cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
				}


			}
			catch (const GenericException &e)
			{
				// Error handling.
				cerr << "An exception occurred." << endl
					<< e.GetDescription() << endl;
				exitCode = 1;
			}
			// Releases all pylon resources. 
			PylonTerminate();
			std::cout << strFileName << endl;
			Motor.move_distance(14);
		}

#endif
#ifdef READ
	vector<cv::Mat> img;
	for (int i = 0; i < pic2Dnum; i++)
	{
		char strFileName[200];
		sprintf(strFileName, "%s/%d%s", strPath2D, i, path_suffix);
		cout << strFileName << endl;
		cv::Mat img1 = cv::imread(strFileName,0);
		img.push_back(img1);
	}
	
	cv::Mat output = socket2D(img);
	cv::Mat result = Detect2d.drawResult(img[1], output);
	cv::namedWindow("2d result", CV_WINDOW_NORMAL);
	cv::imshow("2d result",result);
	cv::waitKey();
/*

	cv::Mat silkModel2d;
	vector<vector<Point>> contoursAl, contoursLiquid;
	std::cout << contoursAl.size() << " and " << contoursLiquid.size() << endl;
	string error2D = Detect2d.scratchCheck(img, silkModel2d, contoursAl, contoursLiquid);
	std::cout << "Error ID for 2D is: " << error2D << endl;
	std::cout << contoursAl.size() << " and " << contoursLiquid.size() << endl;
*/

	//imwrite("D:/silkModel2d.jpg", silkModel2d);
	//imshow("silkModel2d", silkModel2d);
#endif

#ifdef GRAB
		/*
		for (int i = 0; i < 2; i++)
		{
			const char* pathToFlexScan3D = "C:\\Program Files\\LMI Technologies\\FlexScan3D 3.3\\App\\FlexScan3D.exe";

			char* itemName;
			int* numValues;
			float** values;
			if (i == 0)
			{
				cout << "Please put the battery under the LMI camera vertically!" << endl;
			}
			if (i == 1)
			{
				cout << "Please put the battery under the LMI camera horizonlly!" << endl;
			}
			getchar();
			// Start FlexScan3D
			//printf("Starting FlexScan3D...\n");
			//if (FS3D_Init(pathToFlexScan3D) != FS3D_RESULT_OK)
			if (FS3D_Attach() != FS3D_RESULT_OK)
			{
				printf("Could not start FlexScan3D.\n");
				return -1;
			}

			// Disable writing to disk
			//printf("Disable disk writing...\n");
			if (FS3D_Command("scriptline \"Set('Scanning_WriteToDisk', 'False')\"") != FS3D_RESULT_OK)
			{
				printf("An error occurred when modifying the Scanning_WriteToDisk setting.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}

			printf("Connecting to the scanner...\n");
			if (FS3D_Command("scriptline \"ScannerConnect()\"") != FS3D_RESULT_OK)
			{
				printf("An error occurred when attempting to connect to the scanner.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}

			// Set up the callback
			//printf("Register callback...\n");
			// ScanProcessedCallback gets called every time a scan occurs
			if (FS3D_RegisterCallback("ScanProcessed", NULL, ScanProcessedCallback) != FS3D_RESULT_OK)
			{
				printf("An error occurred when registering the callback.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}

			boost::timer constant;

			//printf("Setting scanner exposure...\n");
			if (FS3D_Command("scriptline \"SetScannerExposure(GetScannerNameFromIndex(0), 100)\"") != FS3D_RESULT_OK)
			{
				printf("An error occurred when set scanner exposure.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}

			printf("Scanning...\n");
			if (FS3D_Command("scriptline \"Scan()\"") != FS3D_RESULT_OK)
			{
				printf("An error occurred when attempting to scan.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}
			cout << "Total time: " << constant.elapsed() << endl;


			// Cancel the callback
			//printf("Unregister callback...\n");
			//ScanProcessedCallback gets called every time a scan occurs
			if (FS3D_UnregisterCallback("ScanProcessed") != FS3D_RESULT_OK)
			{
				printf("An error occurred when unregistering the callback.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}


			// Keep running until a key is pressed
			//printf("Press the Enter key to exit...\n");
			//getchar();

			//printf("Enable disk writing...\n");
			if (FS3D_Command("scriptline \"Set('Scanning_WriteToDisk', 'True')\"") != FS3D_RESULT_OK)
			{
				printf("An error occurred when modifying the Scanning_WriteToDisk setting.\n");
				//FS3D_Exit();
				FS3D_Detach();
				return -1;
			}
			// shut down FlexScan3D
			//printf("Shutting down FlexScan3D...\n");
			//FS3D_Exit();
			FS3D_Detach();
			if (i == 0)
			{
				imgdepthVert = cv::imread("cache.jpg");
			}
			if (i == 1)
			{
				imgdepthHor = cv::imread("cache.jpg");
				transpose(imgdepthHor, imgdepthHor);
				flip(imgdepthHor, imgdepthHor, 1);
			}
		}
		cv::imwrite(strPath3DVert, imgdepthVert);
		cv::imwrite(strPath3DHor, imgdepthHor);
		*/
#endif
#ifdef READ
	/*	cout << strPath3DVert << endl;
		cv::Mat imgdepthVert = cv::imread(strPath3DVert, 0);
		cv::Mat output_Vert = socket3D(imgdepthVert);
		cv::Mat result_Vert = Detect3d.drawResult(imgdepthVert, output_Vert);


		cv::Mat imgdepthHor = cv::imread(strPath3DHor, 0);
		cv::Mat output_Hor = socket3D(imgdepthHor);
		cv::Mat result_Hor = Detect3d.drawResult(imgdepthHor, output_Hor);

		cv::imshow("aa", result_Vert);
		cv::imshow("bb", result_Hor);
		cv::waitKey();*/
		
		//int error3D = Detect3d.errorReport(imgdepthVert, imgdepthHor, silkModel2d);
#endif
	}
	
	return 0;
}

