#ifndef __OPTERRORCODE_H__
#define __OPTERRORCODE_H__

#define   OPT_SUCCEED			                           0	 //operation succeed
#define   OPT_ERR_INVALIDHANDLE						 3001001     //invalid handle
#define   OPT_ERR_UNKNOWN                            3001002     //error unknown 
#define   OPT_ERR_INITSERIAL_FAILED                  3001003	 //failed to initialize a serial port
#define   OPT_ERR_RELEASESERIALPORT_FAILED           3001004	 //failed to release a serial port
#define   OPT_ERR_SERIALPORT_UNOPENED                3001005	 //attempt to access an unopened serial port
#define   OPT_ERR_CREATEETHECON_FAILED               3001006     //failed to create an Ethernet connection
#define   OPT_ERR_DESTORYETHECON_FAILED              3001007     //failed to destroy an Ethernet connection
#define   OPT_ERR_SN_NOTFOUND						 3001008     //SN is not found
#define   OPT_ERR_TURNONCH_FAILED                    3001009	 //failed to turn on the specified channel(s)
#define   OPT_ERR_TURNOFFCH_FAILED                   3001010	 //failed to turn off the specified channel(s)
#define   OPT_ERR_SET_INTENSITY_FAILED               3001011     //failed to set the intensity for the specified channel(s)
#define   OPT_ERR_READ_INTENSITY_FAILED              3001012     //failed to read the intensity for the specified channel(s)
#define   OPT_ERR_SET_TRIGGERWIDTH_FAILED	         3001013	 //failed to set trigger pulse width
#define   OPT_ERR_READ_TRIGGERWIDTH_FAILED           3001014	 //failed to read trigger pulse width	
#define   OPT_ERR_READ_HBTRIGGERWIDTH_FAILED         3001015	 //failed to read high brightness trigger pulse width
#define   OPT_ERR_SET_HBTRIGGERWIDTH_FAILED          3001016	 //failed to set high brightness trigger pulse width
#define   OPT_ERR_READ_SN_FAILED                     3001017	 //failed to read serial number
#define   OPT_ERR_READ_IPCONFIG_FAILED               3001018	 //failed to read IP address
#define   OPT_ERR_CHINDEX_OUTRANGE	                 3001019     //index(es) out of the range
#define   OPT_ERR_WRITE_FAILED		                 3001020     //failed to write data
#define   OPT_ERR_PARAM_OUTRANGE	                 3001021     //parameter(s) out of the range 
#define   OPT_ERR_READ_MAC_FAILED					 3001022	 //failed to read MAC
#define   OPT_ERR_SET_MAXCURRENT_FAILED				 3001023	 //failed to set max current
#define	  OPT_ERR_READ_MAXCURRENT_FAILED			 3001024	 //failed to read max current
#define   OPT_ERR_SET_TRIGGERACTIVATION_FAILED		 3001025	 //failed to set trigger activation
#define   OPT_ERR_READ_TRIGGERACTIVATION_FAILED		 3001026	 //failed to read trigger activation
#define   OPT_ERR_SET_WORKMODE_FAILED				 3001027	 //failed to set work mode
#define   OPT_ERR_READ_WORKMODE_FAILED				 3001028	 //failed to read work mode
#define   OPT_ERR_SET_BAUDRATE_FAILED				 3001029	 //failed to set baud rate
#define   OPT_ERR_SET_CHANNELAMOUNT_FAILED			 3001030	 //failed to set channel amount
#define   OPT_ERR_SET_DETECTEDMINLOAD_FAILED		 3001031	 //failed to set detected min load
#define   OPT_ERR_READ_OUTERFREQUENCYUPPERBOUND_FAILED 3001032	 //failed to read max frequency
#define   OPT_ERR_SET_INNERTRIGGERFREQUENCY_FAILED   3001033	 //failed to set inner trigger frequency
#define   OPT_ERR_READ_INNERTRIGGERFREQUENCY_FAILED  3001034	 //failed to read inner trigger frequency
#define   OPT_ERR_SET_DHCP_FAILED					 3001035	 //failed to set DHCP
#define   OPT_ERR_SET_LOADMODE_FAILED				 3001036	 //failed to set load mode
#define   OPT_ERR_READ_PROPERTY_FAILED				 3001037	 //failed to read property
#define   OPT_ERR_CONNECTION_RESET_FAILED			 3001038	 //failed to reset connection
#define	  OPT_ERR_SET_HEARTBEAT_FAILED				 3001039	 //failed to set ethe connection heartbeat
#define   OPT_ERR_GETCONTROLLERLIST_FAILED           3001040     //Failed to get controler(s) list           
#define   OPT_ERR_SOFTWARETRIGGER_FAILED             3001041     //Failed to software trigger                
#define   OPT_ERR_GET_CHANNELSTATE_FAILED            3001042     //Failed to get channelstate          
#define   OPT_ERR_SET_KEEPALIVEPARAMETERS_FAILED     3001043     //Failed to set keepalvie parameters          
#define   OPT_ERR_ENABLE_KEEPALIVE_FAILED            3001044     //Failed to enable/disable keepalive

#endif