//
//  SD_Network.cpp
//  
//
//  Created by Daniel Reeves on 7/15/15.
//
//

#include "SD_Network.h"
#include <SD.h>
#include <EEPROM.h>




SD_Network::SD_Network() {
	strcpy( fileName, "settings.txt");
	SDinitialized = false;

	init();
}



void SD_Network::init() {
	
	// initialize SD card

  	// On the Ethernet Shield, CS is pin 4. It's set as an output by default.
  	// Note that even if it's not used as the CS pin, the hardware SS pin
  	// (10 on most Arduino boards, 53 on the Mega) must be left as an output
  	// or the SD library functions will not work.
	pinMode(10, OUTPUT);
	digitalWrite(10, HIGH);
  	if (!SD.begin(4)) {
		Serial.println(F("SD init fail!"));
		SDinitialized = false;
		}
		else
	{
		Serial.println(F("SD initialized."));
		SDinitialized = true;
	}
	
    // load values from memory if available
	bool loaded = loadSettings();
	
	
    if (!loaded)
    {
	  Serial.println(F("load net settings fail. Using defaults: "));
      doRestoreDefaults();
    }
 
    // print out the values available
	printAll();
    Serial.println(F("Net OPTIONS:  restore  -- set MAC"));
	Serial.println();
}


bool SD_Network::loadSettings(){
	bool success = true;
	
	// load network settings
	if (!SDinitialized) return false;
	Serial.println(F("Loading net settings from SD"));
		networkSettings tempSettings;
	File settingsFile;
	settingsFile = SD.open(fileName, FILE_READ);
	if (settingsFile)
	{
		byte  *buff = (byte *) &tempSettings; // to access example as bytes
		for ( byte count = 0; count < sizeof( networkSettings ); count++ )
	  	{

			if ( settingsFile.available())
			{
		  		*( buff + count ) = settingsFile.read();
			}
			else
			{
		  		Serial.print( F( "Error Reading from settings file" ));
		 		success = false;
			}
	  	}
		settingsFile.close();
		if (success) settings = tempSettings;
	}
	else
	{
		success = false;
		Serial.println(F("error opening settings file"));
	}


	// load MAC
	if (EEPROM.read(0) != 255){
		EEPROM.get(0, MAC);
	}
	else
	{
		Serial.println(F("No data in EEPROM"));
		success = false;
	}
	
	return success;
}


void SD_Network::saveSettings(){
	
	bool failure = false;
	Serial.println(F("SAVING SETTINGS"));

	/// Save MAC
	EEPROM.put(0, MAC);
	Serial.println(F("MAC saved to EEPROM"));

	if (!SDinitialized) return;
		// prep data
	byte    *buff = (byte *) &settings; // to access example as bytes

	// remove old file if it exists
	bool removed = SD.remove(fileName);
	if (removed) Serial.println( F( "Removing Settings" ));

	// open File
	File settingsFile;
	//settingsFile = SD.open(fileName, FILE_WRITE);
	settingsFile = SD.open(fileName, FILE_WRITE);
	if (!settingsFile)
	{
		Serial.println( F("Error: open settings for writing" ));
		failure = true;
	}
	else
	{
		if ( !settingsFile.seek( 0 ))
		{
			failure = true;
		}
		if (!failure){
			byte count = settingsFile.write( buff, sizeof( networkSettings ));
			if ( count != sizeof( networkSettings ))
			{
				failure = true;
			}
		}
		settingsFile.close();
		Serial.println(F("Settings saved to SD"));
	}

    
	
}



void SD_Network::setRemotePort(){
  defaultSettings defset;
  int remote = defset.remotePortBase + settings.ipArray[3];
  settings.remotePortID = remote; 
}


/// Main functions
void SD_Network::doIPSetup()
{
  int numEntered = 0; // number of bytes entered for address
  printIP();
  Serial.println(F("Set Arduino IP - First number:"));
  
  while (numEntered < 4)
  {
   if (Serial.available()) {
       int nextnum = Serial.parseInt();
       settings.ipArray[numEntered] = nextnum;
       numEntered++;
       if (numEntered < 4) Serial.println(F("Next number:"));
    }
  }
  Serial.println(F("IP stored."));
  printIP();
   
  // set remote port appropriately given ip address
  setRemotePort();
  printPorts();
  
  // save to SD
  saveSettings();

}

void SD_Network::doMACSetup()
{ 
  int numEntered = 0; // number of bytes entered for address
  printMAC();
  Serial.println(F("Setting Arduino Mac - Enter first byte:"));
  while (numEntered < 6)
  {
   if (Serial.available()) {
       String nextstring = Serial.readString();
       char* arr = (char*) nextstring.c_str();       
       int nextnum=0;
       sscanf(arr, "%x", &nextnum);
       byte nextbyte = (byte) nextnum;
       MAC[numEntered] = nextbyte;
       numEntered++;
       if (numEntered < 6) Serial.println(F("enter next byte:"));
    }
  }
  Serial.println(F("MAC stored."));
  // save to EEProm
  saveSettings();
  printMAC();
}


void SD_Network::doRestoreDefaults()
{
  Serial.println(F("Restoring The Defaults"));
	
	defaultSettings defset;

	// Set MAC
   for (int i=0;i<6; i++)
   {
     //MAC[i] = defaultMAC[i];
	   float z = 0;
   }
	
  // Set Network Settings variables
   for (int i=0;i<4; i++)
   {
     settings.remoteIpArray[i] = defset.defaultRemoteIP[i];
   }

   for (int i=0;i<4; i++)
   {
     settings.ipArray[i] = defset.defaultIP[i];
   }

   for (int i=0;i<4; i++)
   {
     settings.gateway[i] = defset.defaultGateway[i];
   }
   
   for (int i=0;i<4; i++)
   {
     settings.subnet[i] = defset.defaultSubnet[i];
   }

   settings.localPortID = defset.defaultLocalPort;
  
   setRemotePort();
   
   printAll();
   
   saveSettings();
}




/// -- Utilities
// utility to print to screen IPAddress

void SD_Network::printMAC() {
	Serial.print(F("MAC Address: "));
	for (int i = 0; i<6; i++) {
		Serial.print(MAC[i], HEX);
		Serial.print(" ");
	}
	Serial.println();
}

void SD_Network::printIP() {
	Serial.print(F("IP Address: "));
	for (int i = 0; i<4; i++) {
		Serial.print(settings.ipArray[i]);
		Serial.print(":");
	}
	Serial.println();
}

void SD_Network::printRemoteIP() {
	Serial.print(F("remote IP Address: "));
	for (int i = 0; i<4; i++) {
		Serial.print(settings.remoteIpArray[i]);
		Serial.print(":");
	}
	Serial.println();
}

void SD_Network::printPorts() {
	Serial.print(F("local port: "));
	Serial.println(settings.localPortID);
	Serial.print(F("remote port: "));
	Serial.print(settings.remotePortID);
	Serial.println();
}

/*
void SD_Network::printGateway() {
	Serial.print(F("gateway: "));
	for (int i = 0; i<4; i++) {
		Serial.print(settings.gateway[i]);
		Serial.print(":");
	}
	Serial.println();
}
*/

void SD_Network::printSubnet() {
	Serial.print(F("subnet: "));
	for (int i = 0; i<4; i++) {
		Serial.print(settings.subnet[i]);
		Serial.print(":");
	}
	Serial.println();
}

void SD_Network::printAll() {
	Serial.println(F("--------------"));
	printIP();
	printRemoteIP();
	printMAC();
	printPorts();
	//printGateway();
	printSubnet();

	Serial.println(F("------------"));
}
