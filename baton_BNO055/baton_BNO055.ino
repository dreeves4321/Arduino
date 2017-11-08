
////////////////////////////////////////////////////////////////////////////
//
// Arduino Code for 9DOF based pointer (baton)
// For the Bosch BNO055 fusion board
//
//
//
//  Copyright (c) 2014-2015, Small Design Firm
//
// This sketch reads raw data from the BNO055, does a little processing, and sends it over UDP
//
//   Connections
//   ===========
//   Connect SCL to analog 5
//   Connect SDA to analog 4
//   Connect VDD to 3.3V DC
//   Connect GROUND to common ground
//  


////--- Libraries ----////
/// Custom settings 
#include "baton_base.h"  // sets critical network and sensor toggles and options
#include "SD_Network.h"  // class for setting and storing network settings on an SD card (or EEProm)
// Note that some of the accessible functionality has been commented out in this sketch to save space. 

/// Storage Libraries
#include <EEPROM.h>
#include <SD.h>

/// Sensor Libraries
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

/// Communication Libraries
#include <Wire.h>  // probably want to use my wire_custom!!!
#include <SPI.h>         // needed for Arduino versions later than 0018
#include "Ethernet-NODNS.h"
#include "EthernetUdp-NODNS.h"




//// ---- Variable And Object Definitions and Initializations ---- /////

/// Create the Fusion Sensor Object
int32_t id =  101;
Adafruit_BNO055 bno = Adafruit_BNO055(id);
bool sensorSucceed = false;


//// Ethernet Setup  
// global setting object
SD_Network* SDNetwork = NULL;
networkSettings* netSettings;
  
#ifdef EXPORT_TO_ETHERNET
  // IP address objects
  IPAddress ip;
  IPAddress remoteIp;  // IP of remote computer
  
    
  // default gateway and dns
  byte dnsdefault[] = {8,8,8,8};
 
  // buffers for receiving and sending data
  char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet,
  char SendBuffer[UDP_OUT_PACKET_SIZE]; //buffer for outgoing data
  char CalDataBuffer[UDP_OUT_PACKET_SIZE]; //buffer for outgoing data
  //char calibrationMessage[] = "calibration";
  char pingMessageGood[] = "baton okay";
  char pingMessageBad[] = "baton bad";
  // An EthernetUDP instance to let us send and receive packets over UDP
  EthernetUDP Udp;
#endif

/// Other Variables
imu::Vector<3> euler;
imu::Vector<3> acc;
imu::Quaternion quat;
String inStr;
String outStr;
unsigned long dataT;
float accMag;
bool printUDP = false;

//// ----- Methods ---- ////

/////// ------- SETUP
void setup() {

  //// First start Serial and Ethernet
  #ifdef SERIAL_OUTPUT
     Serial.begin(SERIAL_PORT_SPEED);
     Serial.println(F("Starting Conductors Baton w/ BNO055"));
     Serial.println(F("COMMANDS: restart --  print output -- restore -- set MAC"));
     Serial.println();   
  #endif
 
 
  #ifdef EXPORT_TO_ETHERNET
     SDNetwork = new SD_Network();
     netSettings = &SDNetwork->settings; 

     //set network settings
     ip = IPAddress(netSettings->ipArray[0], netSettings->ipArray[1], netSettings->ipArray[2], netSettings->ipArray[3]); 
     remoteIp = IPAddress(netSettings->remoteIpArray[0], netSettings->remoteIpArray[1], netSettings->remoteIpArray[2], netSettings->remoteIpArray[3]); 
        
     // start the Ethernet and UDP:  
     Ethernet.begin(SDNetwork->MAC, ip, dnsdefault, netSettings->gateway, netSettings->subnet);


     Udp.begin(netSettings->localPortID);
  #endif
  
  /* Initialise the sensor */
  if(bno.begin())
  {
    Serial.print(F("BNO055  detected"));
    sensorSucceed = true;  
    delay(100);
    bno.setExtCrystalUse(true);
    
    /* Display some basic information on this sensor */
    displaySensorStatus();  
  }
  else {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print(F("BNO055 not detected!!"));
    sensorSucceed = false;
  }

   /* remap axes */
  // bno.setAxisRemap(Adafruit_BNO055::REMAP_DEFAULT, Adafruit_BNO055::REMAP_NO_SIGN);
   //bno.setAxisRemap(Adafruit_BNO055::REMAP_X_Y, Adafruit_BNO055::REMAP_XAXIS_SIGN);
   bno.setAxisRemap(Adafruit_BNO055::REMAP_X_Y, Adafruit_BNO055::REMAP_ZAXIS_SIGN);


   //// Setup some variables
   outStr.reserve(UDP_OUT_PACKET_SIZE);


   // apply a delay 
   delay(1000);
}



/// -------  UTILITY FUNCTIONS

void displaySensorStatus(void)
{
  #ifdef SERIAL_OUTPUT    
    uint8_t system_status, self_test_result, system_error;
    bno.getSystemStatus(&system_status, &self_test_result, &system_error);
    Serial.print(F("Self test (0F : good): "));
    Serial.print(self_test_result, HEX);
    Serial.print(F("System error (0 : good): "));
    Serial.print(system_error);
    Serial.println();
  #endif
}

/*
bool checkMagnetometerCalibrated()
{
  uint8_t sysStat, gyroStat, accelStat, magStat;
  bno.getCalibration(&sysStat, &gyroStat, &accelStat, &magStat);
  return (magStat == 3);
}
*/

/*
void printCalibrationStatus()
{
  //Collect Calibration Status
  uint8_t sysStat, gyroStat, accelStat, magStat;
  bno.getCalibration(&sysStat, &gyroStat, &accelStat, &magStat);
  Serial.print("G-A-M: ");
  Serial.print(gyroStat);
  Serial.print(accelStat);
  Serial.print(magStat);
  Serial.println();
}
*/


/////// MAIN LOOP FUNCTIONS
void doPingReply()
{
  #ifdef SERIAL_OUTPUT
    Serial.println("Ping received");
  #endif
  #ifdef EXPORT_TO_ETHERNET  
    Udp.beginPacket(remoteIp, netSettings->remotePortID);
    if (sensorSucceed) Udp.write(pingMessageGood);
    else Udp.write(pingMessageBad);
    Udp.endPacket();
  #endif 
}


void doRestart()
{
  #ifdef SERIAL_OUTPUT
    Serial.println(" ");
    Serial.println(F("---   Restarting  -----"));
    Serial.println(" ");
    Serial.println(" ");
  #endif
  delay(30);
  asm volatile ("  jmp 0");
}



/// ------ MAIN LOOP
void loop() {
  // put your main code here, to run repeatedly:
  // Listen for UDP data over network
  #ifdef EXPORT_TO_ETHERNET
    int packetSize = Udp.parsePacket();
    if (packetSize)
    {
      #ifdef SERIAL_OUTPUT
        Serial.println(F("received message: "));
      #endif

      memset(&packetBuffer[0], 0, sizeof(packetBuffer));
      Udp.read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
      #ifdef SERIAL_OUTPUT
        Serial.println(packetBuffer);
      #endif
      if (strcmp(packetBuffer, "restart") ==0) doRestart();
      if (strcmp(packetBuffer, "hello") ==0) doPingReply();
  }
  #endif

  // Listen for serial data
  if (Serial.available()) {
     inStr = Serial.readStringUntil('\n');
     if (inStr == "restart") doRestart();
     /*if (str == "set IP")
     {
       if (SDNetwork) SDNetwork->doIPSetup();
       doRestart();
     }
     */
     if (inStr == "set MAC")
     {
       if (SDNetwork) SDNetwork->doMACSetup();
       doRestart();
     }
     if (inStr == "restore")
     {
       if (SDNetwork) SDNetwork->doRestoreDefaults();
       doRestart();
     }
     if (inStr == "print output")
     {
        printUDP = !printUDP;
     }
     //if (inStr == "print cal")
     //{
     //   printcal = !printcal;      
     //}
  }

  // print calibration if called for
  //if (printcal) printCalibrationStatus();
  
  // Get and post data only if the sensor started properly
  if (sensorSucceed)
  {
    dataT = millis();
    
    // Get Orientation as Euler angle:  (head - roll - pitch)
    //euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);     

   quat = bno.getQuat();

   // Chck for zeros and restart
   if (quat.w()==0 && quat.x() == 0 && quat.y() == 0 && quat.z() == 0) doRestart();
  
    // Get Acceleration vector
    acc = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);
     
    accMag = acc.magnitude();

 // printCalibrationStatus();


   //// print data
      
    // Send heading, pitch, and mag of accel
    outStr = String(dataT) + " ";
 
    outStr = outStr + String((int)(EXPORT_ANGLE_MULTIPLE * quat.w())) + " " + String((int)(EXPORT_ANGLE_MULTIPLE * quat.x())) + " " + String((int)(EXPORT_ANGLE_MULTIPLE * quat.y())) + " " + String((int)(EXPORT_ANGLE_MULTIPLE * quat.z())) +" " + String((int)(EXPORT_ACC_MULTIPLE * accMag)) + " ";
    //str = str + String((int)(EXPORT_ANGLE_MULTIPLE * euler.x())) + " " + String((int)(EXPORT_ANGLE_MULTIPLE * euler.y())) + " " + String((int)(EXPORT_ANGLE_MULTIPLE * euler.z())) + " " + String((int)(EXPORT_ACC_MULTIPLE * accMag)) + " ";
    //str = str + String((int)(EXPORT_MULTIPLE * y)) + " " + String((int)(EXPORT_MULTIPLE * z)) + " " + String((int)(EXPORT_MULTIPLE * accMag)) + " ";
   

  /// --- export data to ethernet
  #ifdef EXPORT_TO_ETHERNET
      SendBuffer[0] = 0;
      outStr.toCharArray(SendBuffer, UDP_OUT_PACKET_SIZE);
      Udp.beginPacket(remoteIp,     netSettings->remotePortID);
      Udp.write(SendBuffer);
      Udp.endPacket();   
  #endif
  
  #ifdef SERIAL_OUTPUT
      if (printUDP)
      {
        Serial.println(outStr);
      }
  #endif
     
  }
  
  else
  {
    delay(1000);
    doRestart();
  }
 

}
