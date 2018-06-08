// OPTController.h : OPTController DLL 的主头文件
//

#pragma once

/*************************************************
Copyright (C), 2005 OPT MACHINE VISION TECH CO.,LTD. All rights reserved.
Version: 1.0.8          
Date: 2015.03.10         
Description: the set of functions that exported for the controller to control light source, such as turn on/off the light source, 
             adjust the intensity.
History: 
Shengping Wang 2014/10/17 1.0.0 build this moudle
Shengping Wang 2014/11/14 1.0.1 fix bug:#001  
Shengping Wang 2014/11/19 1.0.5 fix bug:#002 #003
Shengping Wang 2014/12/03 1.0.6 fix bug:#004
Jintang	  Mai  2015/03/06 1.0.7 fix bug:#005
Jintang	  Mai  2015/03/10 1.0.8 add new function : OPTController_ConnectionResetByIP OPTController_SetEtheConnectionHeartBeat
Zhouping       2016.05.11 1.1.0 add new function OPTController_IsConnect
ZhongYong      20161014   1.0.12 add new functions and fix bugs
*************************************************/

#ifndef __OPTCONTROLER_H__
#define __OPTCONTROLER_H__

//typedef	  long   OPTController_Handle;

#ifdef _WIN64 //zy 20160809
typedef	  long long   OPTController_Handle;
#else
typedef	  long   OPTController_Handle;
#endif

typedef struct OPTController_IntensityItem
{
	int channelIndex;
	int intensity;
}IntensityItem;

typedef struct OPTController_TriggerWidthItem
{
	int channelIndex;
	int triggerWidth;
}TriggerWidthItem;

typedef struct OPTController_HBTriggerWidthItem
{
	int channelIndex;
	int HBTriggerWidth;
}HBTriggerWidthItem;

typedef struct OPTController_MaxCurrentItem
{
	int channelIndex;
	int maxCurrent;
}MaxCurrentItem;

typedef struct OPTController_SoftwareTriggerItem
{
	int channelIndex;
	int time;
}SoftwareTriggerItem;

extern "C" //ZP
{
	/*******************************************************************
	Function: OPTController_InitSerialPort
	Description:  initialize an available serial port
	Input(s):  
	    comName  -the name of the serial port. e.g., "COM1"
	Output(s): 
	    controllerHandle -the handle of the controller
	Return: 
	    succeed  -OPT_SUCCEED
	    failed   -OPT_ERR_INITSERIAL_FAILED or OPT_ERR_SERIALPORT_UNOPENED (see the error code in  OPTErrorCode.h)
	See also: OPTController_ReleaseSerialPort
	*******************************************************************/
	long  OPTController_InitSerialPort(char *comName, OPTController_Handle *controllerHandle);
	//long OPTController_InitSerialPort(char *comName, OPTController_Handle *controllerHandle);
	/*******************************************************************
	Function: OPTController_ReleaseSerialPort
	Description: release an existing serial port
	Input(s): 
	    controllerHandle -the handle of the controller
	Return:   
	    succeed  -OPT_SUCCEED
		failed   -OPT_ERR_RELEASESERIALPORT_FAILED     
	See also: OPTController_InitSerialPort                                                        
	****************************************************************/
	long  OPTController_ReleaseSerialPort(OPTController_Handle controllerHandle);
	//long OPTController_ReleaseSerialPort(OPTController_Handle controllerHandle);
	/*******************************************************************
	Function: OPTController_CreateEtheConnectionByIP
	Description: create an Ethernet connection by IP address
	Input(s):  
	    serverIPAddress    -the IP of the server. e.g.: IP address of the device which is employed as server. The server IP address can be 127.0.0.1
	Output(s): 
	    controllerHandle   -the handle of the controller
	Return: 
	    succeed   -OPT_SUCCEED
		failed    -OPT_ERR_CREATEETHECON_FAILED        
	See also: OPTController_CancelEtheConnect   
	Remarks:  
	    (1) connect to a server as a client. Before connecting, make sure that the controller is connected to the LAN
		(2) We recommend creating an Ethernet connection by SN (compared with by IP) because IP is likely to be changed dynamically in LAN 
	        under the DHCP protocol. We have provided a tool (SearchForControllers.exe) to check SN.
	*******************************************************************/
	long  OPTController_CreateEtheConnectionByIP(char *serverIPAddress, OPTController_Handle *controllerHandle);
	//long OPTController_CreateEtheConnectionByIP(char *serverIPAddress, OPTController_Handle *controllerHandle);
	/*******************************************************************
	Function: OPTController_CreateEtheConnectionBySN
	Description: create an Ethernet connection by serial number
	Input(s): 
	    serialNumber     -the serial number of the controller 
	Output(s): 
	    controllerHandle -the handle of the controller
	Return: 
	    succeed   -OPT_ SUCCEED
	    failed    -OPT_ERR_CREATEETHECON_FAILED 
	See also: OPTController_DestroyEtheConnect  
	Remarks:  
	    (1) connect to a server as a client. Before connecting, make sure that the controller is connected to the LAN
	    (2) We recommend creating an Ethernet connection by SN (compared with by IP) because IP is likely to be changed dynamically in LAN 
	        under the DHCP protocol. We have provided a tool (SearchForControllers.exe) to check SN.
	*******************************************************************/
	long  OPTController_CreateEtheConnectionBySN(char *serialNumber, OPTController_Handle *controllerHandle);
	//long OPTController_CreateEtheConnectionBySN(char *serialNumber, OPTController_Handle *controllerHandle);
	/*****************************************************************
	Function: OPTController_DestoryEtheConnection
	Description: destroy an existing Ethernet Connection
	Input(s):    
	    controllerHandle  -the handle of the controller
	Return: 
	    succeed   -OPT_SUCCEED
	    failed    -OPT_ERR_DESTORYETHECON_FAILED
	*****************************************************************/
	long  OPTController_DestoryEtheConnection(OPTController_Handle controllerHandle);
	//long OPTController_DestoryEtheConnection(OPTController_Handle controllerHandle);
	/*******************************************************************
	Function: OPTController_TurnOnChannel
	Description:turn on the specified channel(s)
	Input(s):
	    controllerHandle -the handle of controller
	    channelIndex     -the index(es) of the channel(s) to be turned on, range:[0-16] (in decimal form, 0 for all channels)
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_TURNONCH_FAILED
	See also: OPTController_TurnOffChannel           
	*******************************************************************/
	long  OPTController_TurnOnChannel(OPTController_Handle controllerHandle,int channelIndex);
	//long OPTController_TurnOnChannel(OPTController_Handle controllerHandle,int channelIndex);
	/*******************************************************************
	Function: OPTController_TurnOnMultiChannel
	Description: turn on the specified multiple channels
	Input(s):
	    controllerHandle   -the handle of controller
	    channelIndexArray  -an array consists of the indexes of the channels to be turned on, range:[1-16] (in decimal form)
		length             -the length of the channel index array
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_TURNONCH_FAILED
	See also: OPTController_TurnOffMultiChannel              
	*******************************************************************/
	long  OPTController_TurnOnMultiChannel(OPTController_Handle controllerHandle,int* channelIndexArray, int length);
	//long OPTController_TurnOnMultiChannel(OPTController_Handle controllerHandle,int* channelIndexArray, int length);
	/*******************************************************************
	Function: OPTController_TurnOffChannel
	Description: turn off the specified channel(s)
	Input(s):
	    controllerHandle -the handle of controller
	    channelIndex     -the index(es) of the channel(s) to be turned off, range:[0-16] (in decimal form, 0 for all channels);
	Return:
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_TURNOFFCH_FAILED
    See also: OPTController_TurnOnChannel  
	*******************************************************************/
	long  OPTController_TurnOffChannel(OPTController_Handle controllerHandle,int channelIndex);
	//long OPTController_TurnOffChannel(OPTController_Handle controllerHandle,int channelIndex);
	/*******************************************************************
	Function: OPTController_TurnOffMultiChannel
	Description: turn off the specified multiple channels
	Input(s):
	    controllerHandle  -the handle of controller;
	    channelIndexArray -an array consists of the indexes of the channels to be turned off, range:[1-16] (in decimal form);
		length            -the length of the channel index array
	Return: 
	    succeed    -OPT_SUCCEED;
	    failed     -OPT_ERR_TURNOFFCH_FAILED
    See also: OPTController_TurnOnMultiChannel 
	*******************************************************************/
	long  OPTController_TurnOffMultiChannel(OPTController_Handle controllerHandle,int* channelIndexArray, int length);
	//long OPTController_TurnOffMultiChannel(OPTController_Handle controllerHandle,int* channelIndexArray, int length);
	/*******************************************************************
	Function: SetIntensity
	Description: set intensity for the specified channel(s)
	Input(s):
	    controllerHandle -the handle of controller
	    channelIndex     -the index(es) of the channel(s), range:[0-16] (in decimal form, 0 for all channels)
	    intensity        -the intensity value, range: [0-255] (in decimal form)
	Return:   
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_SET_INTENSITY_FAILED
    See also: OPTController_ReadIntensity
	*******************************************************************/
	long  OPTController_SetIntensity (OPTController_Handle controllerHandle,int channelIndex, int intensity);
	//long OPTController_SetIntensity (OPTController_Handle controllerHandle,int channelIndex, int intensity);
	/*******************************************************************
	Function: SetMultiIntensity
	Description: set intensities for the specified multiple  channels
	Input(s):
	    controllerHandle -the handle of controller
	    intensityArray   -an array consists of the intensities (and the indexes of corresponding channels) to be set, range: [0-255] (in decimal form)
		length           -the length of the intensity array
	Return:   
	    succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_INTENSITY_FAILED    
    See also: OPTController_ReadIntensity
	*******************************************************************/
	long  OPTController_SetMultiIntensity (OPTController_Handle controllerHandle,IntensityItem* intensityArray, int length);
	//long OPTController_SetMultiIntensity (OPTController_Handle controllerHandle,IntensityItem* intensityArray, int length);
	/*******************************************************************
	Function: ReadIntensity
	Description: read intensity of the specified channel
	Input(s):
	controllerHandle -the handle of controller
	channelIndex     -the index of the channel, range: [1-16] (in decimal form)
	Output(s): 
	    intensity    -the obtained intensity value;
	Return: 
	    succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_INTENSITY_FAILED   
	See also: OPTController_SetIntensity & OPTController_SetMultiIntensity
	****************************************************************/
	long  OPTController_ReadIntensity (OPTController_Handle controllerHandle,int channelIndex, int *intensity);
	//long OPTController_ReadIntensity (OPTController_Handle controllerHandle,int channelIndex, int *intensity);
	/*******************************************************************
	Function: SetTriggerWidth
	Description: set trigger pulse width for corresponding channel(s)
	Input(s):
	    controllerHandle  -the handle of controller
	    channelIndex      -the index(es) of the channel(s), range:[0-16] (in decimal form, 0 for all channels)
	    triggerWidth      -the value of the trigger pulse width to be set, range:[1-999]
	Return: 
	    succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_TRIGGERWIDTH_FAILED        
	See also: OPTController_ReadTriggerWidth
	******************************************************************/
	long  OPTController_SetTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int triggerWidth);
	//long OPTController_SetTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int triggerWidth);
	/*******************************************************************
	Function: SetMultiTriggerWidth
	Description: set trigger pulse width for the specified multiple  channels simultaneously 
	Input(s):
	    controllerHandle   -the handle of controller
	    triggerWidthArray  -an array consists of values of the trigger pulse width (and the indexes of corresponding channels) to be set, range:[1-999]
		length             -the length of the trigger width array
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_SET_TRIGGERWIDTH_FAILED
    See also: OPTController_ReadTriggerWidth
	******************************************************************/
	long  OPTController_SetMultiTriggerWidth(OPTController_Handle controllerHandle,TriggerWidthItem* triggerWidthArray, int length);
	//long OPTController_SetMultiTriggerWidth(OPTController_Handle controllerHandle,TriggerWidthItem* triggerWidthArray, int length);
	/*******************************************************************
	Function: ReadTriggerWidth
	Description: read the trigger pulse width of the specified channel
	Input(s):
	    controllerHandle  -the handle of controller;
	    channelIndex      -the index of the channel, range:[1-16] (in decimal form)
	Output(s):    
	    triggerWidth      -the obtained trigger pulse width;
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_SET_TRIGGERWIDTH_FAILED	    
	See also: OPTController_SetTriggerWidth & OPTController_SetMultiTriggerWidth
	****************************************************************/
	long  OPTController_ReadTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int* triggerWidth);
	//long OPTController_ReadTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int* triggerWidth);
	/*******************************************************************
	Function: SetHBTriggerWidth
	Description: set the high brightness trigger pulse width for corresponding channel(s);
	Input(s):
	    controllerHandle  -the handle of controller;
	    channelIndex      -the index(es) of the channel(s), range:[0-16] (in decimal form, 0 for all channels);
	    HBTriggerWidth    -the value of the high brightness trigger pulse width to be set, range:[1-500].
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_SET_HBTRIGGERWIDTH_FAILED
	See also: OPTController_ReadHBTriggerWidth
	****************************************************************/
	long  OPTController_SetHBTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int HBTriggerWidth);
	//long OPTController_SetHBTriggerWidth(OPTController_Handle controllerHandle,int channelIndex, int HBTriggerWidth);
	/*******************************************************************
	Function: SetMultiHBTriggerWidth
	Description: set high brightness trigger pulse width for the specified multiple channels
	Input(s):
	    controllerHandle     -the handle of controller;
	    HBtriggerWidthArray  -an array consists of values of the high brightness trigger pulse width (and the indexes of corresponding channels) 
		                      to be set, range:[1-500]
		length               -the length of the high brightness trigger width array
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_SET_HBTRIGGERWIDTH_FAILED
    See also: OPTController_ReadHBTriggerWidth
	****************************************************************/
	long  OPTController_SetMultiHBTriggerWidth(OPTController_Handle controllerHandle,HBTriggerWidthItem* HBtriggerWidthArray, int length);
	//long OPTController_SetMultiHBTriggerWidth(OPTController_Handle controllerHandle,HBTriggerWidthItem* HBtriggerWidthArray, int length);
	/*******************************************************************
	Function：ReadHBTriggerWidth
	Description: read the high brightness trigger pulse width of the specified channel
	Input(s):
	    controllerHandle  -the handle of controller
	    channelIndex      -the index of the channel, range:[1-16] (in decimal form)
	Output(s):  	 
	    HBTriggerWidth    -the obtained high brightness trigger pulse width
	Return: 
	    succeed    -OPT_SUCCEED
 	    failed     -OPT_ERR_READ_HBTRIGGERWIDTH_FAILED
    See also: OPTController_SetHBTriggerWidth & OPTController_SetMultiHBTriggerWidth
	*****************************************************************/
	long  OPTController_ReadHBTriggerWidth(OPTController_Handle controllerHandle,int channelIndex,int* HBTriggerWidth);
	//long OPTController_ReadHBTriggerWidth(OPTController_Handle controllerHandle,int channelIndex,int* HBTriggerWidth);
	/*******************************************************************
	Function: OPTController_EnableResponse
	Description:  to set whether return value are needed or not
	Input:
		controllerHandle  -the handle of controller
		isResponse        -"true" means "need response" while "false" stands for not
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_UNKNOWN
	*****************************************************************/
	long  OPTController_EnableResponse(OPTController_Handle controllerHandle,bool isResponse);
	//long OPTController_EnableResponse(OPTController_Handle controllerHandle,bool isResponse);
	/*******************************************************************
	Function: OPTController_EnableCheckSum
	Description: to set whether checksum are needed or not
	Input:
	    controllerHandle  -the handle of controller
	    isCheckSum        -"true" means "need checksum" while "false" stands for not
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_UNKNOWN
	*****************************************************************/
	long  OPTController_EnableCheckSum(OPTController_Handle controllerHandle, bool isCheckSum);
	//long OPTController_EnableCheckSum(OPTController_Handle controllerHandle, bool isCheckSum);
	/*******************************************************************
	Function：OPTController_EnablePowerOffBackup
	Description:  to set whether backup are needed or not in the case of power off
	Input:
		controllerHandle   -the handle of controller
		isSave:            -"true" means "need back up" while "false" stands for not
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_UNKNOWN
	*****************************************************************/
	long  OPTController_EnablePowerOffBackup(OPTController_Handle controllerHandle,bool isSave);
	//long OPTController_EnablePowerOffBackup(OPTController_Handle controllerHandle,bool isSave);
	/*******************************************************************
	Function: OPTController_ReadSN
	Description: read the serial number (SN) of the controller
	Input:
		controllerHandle  -the handle of controller
	Output:
		SN 	              -the obtained serial number
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_READ_SN_FAILED
	*****************************************************************/
	long  OPTController_ReadSN(OPTController_Handle controllerHandle, char *SN);
	//long OPTController_ReadSN(OPTController_Handle controllerHandle, char *SN);
	/*******************************************************************
	Function: OPTController_ReadIPConfig
	Description: read IP configuration of the controller
	Input:
		controllerHandle	 -the handle of controller
	Output:
		IP                   -the obtained IP address
		subnetMask           -the obtained subnet mask
		defaultGateway       -the obtained default gateway
	Return: 
	    succeed    -OPT_SUCCEED
	    failed     -OPT_ERR_READ_IPCONFIG_FAILED
	*****************************************************************/
	long  OPTController_ReadIPConfig(OPTController_Handle controllerHandle, char *IP, char *subnetMask, char *defaultGateway);
	//long OPTController_ReadIPConfig(OPTController_Handle controllerHandle, char *IP, char *subnetMask, char *defaultGateway);
//////////////
	/*******************************************************************
	Function: OPTController_SetMaxCurrent
	Description: set maximum current for the specified channel	
	Input:
		controllerHandle	-the handle of controller
		channelIndex		-the index(es) of the channel(s), range:[0-16] (in decimal form, 0 for all channels);
		current				-the value of the maximum current to be set, range:[1-100].
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_MAXCURRENT_FAILED
	*****************************************************************/
	long  OPTController_SetMaxCurrent (OPTController_Handle controllerHandle, int channelIndex, int current);
	//long OPTController_SetMaxCurrent (OPTController_Handle controllerHandle, int channelIndex, int current);
	/*******************************************************************
	Function: OPTController_ReadMaxCurrent
	Description: read maximum current for the specified channel	
	Input:
		controllerHandle	-the handle of controller
		channelIndex		-the index of the channel, range:[1-16] (in decimal form)
	Output(s):  	 
		current    -the obtained maximum current
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_MAXCURRENT_FAILED
	*****************************************************************/
	long  OPTController_ReadMaxCurrent (OPTController_Handle controllerHandle, int channelIndex, int *current);
	//long OPTController_ReadMaxCurrent (OPTController_Handle controllerHandle, int channelIndex, int *current);
	/*******************************************************************
	Function: OPTController_SetMultiMaxCurrent
	Description: set maximum current for the specified multiple channels
	Input(s):
		controllerHandle     -the handle of controller;
		maxCurrentArray		 -an array consists of values of the maximum current (and the indexes of corresponding channels) 
							 to be set, range:[1-100]
	length					 -the length of the maximum current array
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_MAXCURRENT_FAILED	
	****************************************************************/
	long  OPTController_SetMultiMaxCurrent(OPTController_Handle controllerHandle, MaxCurrentItem *maxCurrentArray, int length);
	//long OPTController_SetMultiMaxCurrent(OPTController_Handle controllerHandle, MaxCurrentItem *maxCurrentArray, int length);
	/*******************************************************************
	Function: OPTController_ReadMAC
	Description: read the media access control (MAC) address of the controller
	Input:
		controllerHandle	 -the handle of controller
	Output:
		MAC 				 -the obtained media access control
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_MAC_FAILED
	*****************************************************************/
	long  OPTController_ReadMAC(OPTController_Handle controllerHandle, char *MAC);
	//long OPTController_ReadMAC(OPTController_Handle controllerHandle, char *MAC);
	/*******************************************************************
	Function: OPTController_SetTriggerActivation
	Description: set the trigger activation of the controller
	Input:
		controllerHandle		-the handle of controller	
		triggerActivation		-the value of the trigger activation to be set, range:[0-3].
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_TRIGGERACTIVATION_FAILED
	*****************************************************************/
	long  OPTController_SetTriggerActivation(OPTController_Handle controllerHandle, int triggerActivation);
	//long OPTController_SetTriggerActivation(OPTController_Handle controllerHandle, int triggerActivation);
	/*******************************************************************
	Function: OPTController_ReadTriggerActivation
	Description: read the trigger activation of the controller
	Input:
		controllerHandle	-the handle of controller
	Output:
		triggerActivation	-the obtained trigger activation
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_TRIGGERACTIVATION_FAILED
	*****************************************************************/
	long  OPTController_ReadTriggerActivation(OPTController_Handle controllerHandle, int *triggerActivation);
	//long OPTController_ReadTriggerActivation(OPTController_Handle controllerHandle, int *triggerActivation);
	/*******************************************************************
	Function: OPTController_SetWorkMode
	Description: set the work mode of the controller
	Input:
		controllerHandle		-the handle of controller	
		workMode				-the value of the work mode to be set, range:[0-2].
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_WORKMODE_FAILED
	*****************************************************************/
	long  OPTController_SetWorkMode(OPTController_Handle controllerHandle, int workMode);
	//long OPTController_SetWorkMode(OPTController_Handle controllerHandle, int workMode);

	/*******************************************************************
	Function: OPTController_ReadWorkMode
	Description: read the work mode of the controller
	Input:
		controllerHandle		-the handle of controller
	Output:
		workMode				-the obtained work mode
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_WORKMODE_FAILED
	*****************************************************************/
	long  OPTController_ReadWorkMode(OPTController_Handle controllerHandle, int *workMode);
	//long OPTController_ReadWorkMode(OPTController_Handle controllerHandle, int *workMode);
	/*******************************************************************
	Function: OPTController_ReadOuterFrequencyUpperBound
	Description: read the upper bound of the outer frequency of the controller
	Input:
		controllerHandle		-the handle of controller
		channelIndex			-the index(es) of the channel(s), range:[1-16] (in decimal form);
	Output:
		maxFrequency			-the obtained maximum frequency
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_MAXFREQUENCY_FAILED
	*****************************************************************/
	long  OPTController_ReadOuterFrequencyUpperBound(OPTController_Handle controllerHandle, int channelIndex,int *maxFrequency);
	//long OPTController_ReadOuterFrequencyUpperBound(OPTController_Handle controllerHandle, int channelIndex,int *maxFrequency);
	/*******************************************************************
	Function: OPTController_AutoDetectLoadOnce
	Description: auto detect the load of the controller once
	Input:
		controllerHandle		-the handle of controller	
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_WRITE_FAILED
	*****************************************************************/
	long  OPTController_AutoDetectLoadOnce(OPTController_Handle controllerHandle);
	//long OPTController_AutoDetectLoadOnce(OPTController_Handle controllerHandle);
	/*******************************************************************
	Function: OPTController_SetInnerTriggerFrequency
	Description: set the inner trigger frequency of the controller
	Input:
		controllerHandle		-the handle of controller	
		channelIndex			-the index(es) of the channel(s), range:[0-16] (in decimal form, 0 for all channels);
		frequency				-the value of the frequency to be set, range:[15-1000].
	Return: 
	succeed    -OPT_SUCCEED
	failed     -OPT_ERR_SET_INNERTRIGGERFREQUENCY_FAILED
	*****************************************************************/
	long  OPTController_SetInnerTriggerFrequency(OPTController_Handle controllerHandle, int channelIndex, int frequency);
	//long OPTController_SetInnerTriggerFrequency(OPTController_Handle controllerHandle, int channelIndex, int frequency);
	/*******************************************************************
	Function: OPTController_ReadInnerTriggerFrequency
	Description: read the inner trigger frequency of the controller
	Input:
		controllerHandle		-the handle of controller	
		channelIndex			-the index(es) of the channel(s), range:[0-16] (in decimal form);
	Output:
		frequency				-the obtained frequency
	Return: 
	succeed    -OPT_SUCCEED
	failed     -OPT_ERR_READ_INNERTRIGGERFREQUENCY_FAILED
	*****************************************************************/
	long  OPTController_ReadInnerTriggerFrequency(OPTController_Handle controllerHandle, int channelIndex, int *frequency);
	//long OPTController_ReadInnerTriggerFrequency(OPTController_Handle controllerHandle, int channelIndex, int *frequency);
	/*******************************************************************
	Function: OPTController_EnableDHCP
	Description: to enable the Dynamic Host Configuration Protocol(DHCP) or disable it
	Input:
		controllerHandle		-the handle of controller
		bDHCP					-"TRUE" means "enable DHCP" while "FALSE" stands for disable
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_DHCP_FAILED
	*****************************************************************/
	long  OPTController_EnableDHCP(OPTController_Handle controllerHandle, BOOL bDHCP);
	//long OPTController_EnableDHCP(OPTController_Handle controllerHandle, BOOL bDHCP);
	/*******************************************************************
	Function: OPTController_SetLoadMode
	Description: set the load mode of the controller
	Input:
		controllerHandle		-the handle of controller			
		loadMode				-the value of the load mode to be set, 0 for aoto, 1 for manual.
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_LOADMODE_FAILED
	*****************************************************************/
	long  OPTController_SetLoadMode(OPTController_Handle controllerHandle, int loadMode);
	//long OPTController_SetLoadMode(OPTController_Handle controllerHandle, int loadMode);
	/*******************************************************************
	Function: OPTController_ReadProperties
	Description: read the property of the controller
	Input:
		controllerHandle		-the handle of controller
		property				-the code of the property to be read, 1 for model, 2 for firmware version.
	Output:
		value					-the obtained property
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_READ_PROPERTY_FAILED
	*****************************************************************/
	long  OPTController_ReadProperties(OPTController_Handle controllerHandle, int property, char *value);
	//long OPTController_ReadProperties(OPTController_Handle controllerHandle, int property, char *value);
	/*******************************************************************
	Function: OPTController_GetVersion
	Description: get version of the controller dll	
	Output:
		version					-the version of the controller dll
	Return: 
	succeed    -OPT_SUCCEED
	failed     -OPT_ERR_READ_IPCONFIG_FAILED
	*****************************************************************/
	long  OPTController_GetVersion(char *version);
	//long OPTController_GetVersion(char *version);
	/*******************************************************************
	Function: OPTController_ConnectionResetBySN
	Description: reset ethe connection	
	Input:
		serialNumber		-the serial number of controller
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_CONNECTION_RESET_FAILED
	*****************************************************************/
	long  OPTController_ConnectionResetBySN(char *serialNumber);
	//long OPTController_ConnectionResetBySN(char *serialNumber);
	/*******************************************************************
	Function: OPTController_ConnectionResetByIP
	Description: reset ethe connection	
	Input:
		serverIPAddress		-the IP address of controller
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_CONNECTION_RESET_FAILED
	*****************************************************************/
	long  OPTController_ConnectionResetByIP(char *serverIPAddress);
	//long OPTController_ConnectionResetByIP(char *serverIPAddress);
	/*******************************************************************
	Function: OPTController_SetEtheConnectionHeartBeat
	Description: set ethe connection heartbeat	
	Input:
		controllerHandle		-the handle of controller
		timeout					-heartbeat timeout
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_HEARTBEAT_FAILED
	*****************************************************************/
	long  OPTController_SetEtheConnectionHeartBeat(OPTController_Handle controllerHandle, unsigned timeout);
	//long OPTController_SetEtheConnectionHeartBeat(OPTController_Handle controllerHandle, unsigned timeout);
    /****************************************************************
	Function: OPTController_IsConnect(OPTController_Handle controllerHandle)
	Description: check Ethe Connect state
	Input:
	controllerHandle		-the handle of controller
	Return: 
	succeed    - OPT_SUCCEED (0)
	failed     - -1

	****************************************************************/
    long  OPTController_IsConnect(OPTController_Handle controllerHandle); //ZP

	/****************************************************************
	Function: OPTController_ReadChannelState(OPTController_Handle controllerHandle,int ChannelIndex)
	Description: check channel state
	Input:
	controllerHandle		-the handle of controller
    ChannelIndex            -the channel
	Return: 
	succeed    - OPT_SUCCEED (0) 
	failed     - OPT_ERR_CHANNEL_STATE_FAILED

	****************************************************************/
    long  OPTController_ReadChannelState(OPTController_Handle controllerHandle,int ChannelIndex);

    /****************************************************************
	Function: OPTController_GetControllerListOnEthernet(char *snList);
	Description: search for the online controllers, for ethernet  only.
	Input:
	snList	   -the point of char，should new by user and make sure the length is enough。 
	Return: 
	succeed    - OPT_SUCCEED (0) 
	failed     - OPT_ERR_GETCONTROLLERLIST_FAILED

	****************************************************************/
   long  OPTController_GetControllerListOnEthernet(char *snList); //zy

   /*******************************************************************
	Function: OPTController_GetChannelState
	Description: get channel state	
	Input:
	   controllerHandle		-the handle of controller
	   channelIndex			-the index(es) of the channel(s), range:[0-16] (in decimal form);
    Output:
	   state				-channel state; 0 --Light sources connected
	                                        1 --Light sources disconnected
											2 --Short circuit protection
											3 --Over voltage protection
											4 --Over current protection
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_GET_CHANNELSTATE_FAILED
	*****************************************************************/
	long  OPTController_GetChannelState(OPTController_Handle controllerHandle, int channelIdx, int *state);


	/*******************************************************************
	Function: OPTController_SetKeepaliveParameter
	Description: set keepalive parameter	
	Input:
	   controllerHandle		-the handle of controller
	   keepalive_time		-idle time,range:[1-65535] (in decimal form),Unit:seconds;
	   keepalive_intvl		-interval between two keepalive_probes,range:[1-65535] (in decimal form),Unit:seconds;
       keepalive_probes     -probes of keepalive range:[1-65535]
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SET_KEEPALIVEPARAMETERS_FAILED
	*****************************************************************/
	long  OPTController_SetKeepaliveParameter(OPTController_Handle controllerHandle, int keepalive_time,
		int keepalive_intvl, int keepalive_probes);

	/*******************************************************************
	Function: OPTController_EnableKeepalive
	Description: to enable the keepalive or disable it
	Input:
		controllerHandle		-the handle of controller
		enable					-"TRUE" means "enable keepalive" while "FALSE" stands for disable
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_ENABLE_KEEPALIVE_FAILED
	*****************************************************************/
    long  OPTController_EnableKeepalive(OPTController_Handle controllerHandle, BOOL enable);

   /*******************************************************************
	Function:OPTController_SoftwareTrigger
	Description: software trigger,specified channel on specified time.
	Input:
	   controllerHandle		-the handle of controller
	   channelIdx			-the index(es) of the channel(s), range:[0-16] (in decimal form);
	   time					-light duration,range:[1-1000],Unit:10ms
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SOFTWARETRIGGER_FAILED
	*****************************************************************/
	long  OPTController_SoftwareTrigger(OPTController_Handle controllerHandle, int channelIdx, int time);

	/*******************************************************************
	Function: OPTController_MultiSoftwareTrigger
	Description: software trigger for the specified multiple channels
	Input(s):
		controllerHandle       -the handle of controller;
		softwareTriggerArray   -an array consists of values of the software trigger (and the indexes of corresponding channels) 
							 to be set, range:[1-1000],Unit:10ms
	    length				   -the length of the softwareTrigger array
	Return: 
		succeed    -OPT_SUCCEED
		failed     -OPT_ERR_SOFTWARETRIGGER_FAILED	
	****************************************************************/
    long  OPTController_MultiSoftwareTrigger(OPTController_Handle controllerHandle, SoftwareTriggerItem* softwareTriggerArray, int length);


}; //ZP

#endif // __OPTCONTROLER_H__





