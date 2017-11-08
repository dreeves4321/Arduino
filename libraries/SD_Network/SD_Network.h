//
//  SD_Network.h
//  
//
//  Created by Daniel Reeves on 7/15/15.
//  Settings can be changed using Serial input on the Arduino, or can be changed on a computer by accessing the file on the SD card itself. The latter is MUCH easier.
//

#ifndef ____SD_Network__
#define ____SD_Network__

#include "Arduino.h"
#include "baton_base.h"




// define struct
typedef struct networkSettings{
  int ipArray[4];
  int remoteIpArray[4]; 
  byte gateway[4];
  byte subnet[4];
  int remotePortID;
  int localPortID;
} networkSettings;


struct defaultSettings{
	// define defaults (i.e. "restore" settings)
	int defaultIP[4] = { 192, 168, 11, 143};
	int defaultRemoteIP[4] = { 192, 168, 11, 10 };
	byte defaultGateway[4] = { 192,168,0,1 };
	byte defaultSubnet[4] = { 255, 255, 255, 0 };
	int defaultLocalPort = 8888;
	int remotePortBase = 8700;
};



class SD_Network
{
public:
	SD_Network();
	void init();
	
	bool loadSettings();
	void saveSettings();
	
	void doRestoreDefaults();
	
	void doMACSetup();
	void doIPSetup();
	
	// background setting
	void setRemotePort();
	
	// printing
	void printAll();
	void printIP();
	void printRemoteIP();
	void printMAC();
	void printPorts();
	//void printGateway();
	void printSubnet();

public:
	networkSettings settings;
  	byte MAC[6] ;

	char   fileName[14];
	bool   SDinitialized;
	//int    EEPROMoffset = NETSETTINGS_EEPROM_OFFSET;  // offset of ip value starts after callibdata

	// define defaults (i.e. "restore" settings)
	// MAC address
	byte defaultMAC[6] = { 0x90, 0xA2, 0xDA, 0x0F, 0xBF, 0x0F };


	
	
};


#endif /* defined(____SD_Network__) */
