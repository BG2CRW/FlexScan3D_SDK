#ifndef CONFIG_HPP
#define CONFIG_HPP

#define GRAB          //take photos and save
//#define READ        //read photos and test algorithm 
//#define ONLINE     //take photos and online show result


//#define CAM2D
#define CAM3D

#ifdef ONLINE
	#define GRAB 
	#define READ  
#endif
#endif
