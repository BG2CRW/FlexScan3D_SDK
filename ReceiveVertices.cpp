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
#include <string>
// Include files to use the PYLON API.
#include "pylon\PylonIncludes.h"
#include <pylon\PylonGUI.h>
#include "config.hpp"

using namespace std;
using namespace Pylon;
using namespace cv;
static const uint32_t c_countOfImagesToGrab = 1;
class detect2d Detect2d;
class detect3d Detect3d;
cv::Mat imgdepthVert;
cv::Mat imgdepthHor;
cv::Mat imageBasler;
char path3D_prefix[]= "D:/Data/3D/";
char path2D_prefix[] = "D:/Data/2D/";
char path_suffix[] = ".jpg";
char path_3DHor_suffix[] = "_Hor.jpg";
char path_3DVert_suffix[] = "_Vert.jpg";
int counter = 1000;
int errorReport;
int main(int argc, char* argv[])
{
	
	while (1)
	{
		counter++;
		cout << "counter:"<<counter << endl;
		char str0[4];
		sprintf(str0, "%04d", counter);
		char strPath2D[200], strPath3DVert[200], strPath3DHor[200];
		sprintf(strPath2D, "%s%s%s", path2D_prefix, str0, path_suffix);
		sprintf(strPath3DVert, "%s%s%s", path3D_prefix, str0, path_3DVert_suffix);
		sprintf(strPath3DHor, "%s%s%s", path3D_prefix, str0, path_3DHor_suffix);

#ifdef GRAB
		// The exit code of the sample application.
		int exitCode = 0;
		
		// Before using any pylon methods, the pylon runtime must be initialized. 
		PylonInitialize();
		cout << "Please put a side of the next battery in basler!" << endl;
		getchar();
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
				cout << "SizeX: " << ptrGrabResult->GetWidth()<< "SizeY: " << ptrGrabResult->GetHeight() << endl;
				const uint8_t *pImageBuffer = (uint8_t *)ptrGrabResult->GetBuffer();
				//cout << "Gray value of first pixel: " << (uint32_t)pImageBuffer[0] << endl << endl;

				imageBasler = cv::Mat(ptrGrabResult->GetHeight(), ptrGrabResult->GetWidth(), CV_8UC1, (uint8_t *)ptrGrabResult->GetBuffer());
				cv::imwrite(strPath2D, imageBasler);

				boost::timer t2;
				cout << "Time of scratch check: " << t2.elapsed() << endl;
			}
			else
			{
				cout << "Error: " << ptrGrabResult->GetErrorCode() << " " << ptrGrabResult->GetErrorDescription() << endl;
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
		cout << strPath2D << endl;
		

#endif
#ifdef READ
		cv::Mat img = cv::imread(strPath2D,0);
		cv::Mat silkModel2d;
		vector<vector<Point>> contoursAl, contoursLiquid;
		cout << contoursAl.size() << " and " << contoursLiquid.size() << endl;
		string error2D = Detect2d.scratchCheck(img, silkModel2d, contoursAl, contoursLiquid);
		cout << "Error ID for 2D is: " << error2D << endl;
		cout << contoursAl.size() << " and " << contoursLiquid.size() << endl;
		//imwrite("D:/silkModel2d.jpg", silkModel2d);
		//imshow("silkModel2d", silkModel2d);
#endif

#ifdef GRAB
		
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
#endif
#ifdef READ
		cv::Mat imgdepthVert = cv::imread(strPath3DVert, 0);
		cv::Mat imgdepthHor = cv::imread(strPath3DHor, 0);
		//int error3D = Detect3d.errorReport(imgdepthVert, imgdepthHor, silkModel2d);
#endif
	}
	return 0;
}

