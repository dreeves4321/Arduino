#include "serialCon.h"

void serialCon::init(){
 // start serial port 
   Serial.begin(SERIAL_SPEED);
   while (!Serial) {
     ; // wait for serial port to connect. Needed for native USB port only
   }   
   Serial.setTimeout(50);
   printInstructions();

   delay(10);
}

/// Serial Functions
void serialCon::setID(String _string){
	mID = _string;
}

void serialCon::endInstruction(){
	reading = false;
	Serial.println(F("endInstructions"));
}

uint8_t serialCon::shiftAsciiInput(char c){
	uint8_t in = c;
	uint8_t out = 8*(in - 32) / 3 ;
	return out;
}

void serialCon::sendID(){
	if (debugMode) Serial.println();
	Serial.print(F("ID:"));
	Serial.println(mID);
	if (debugMode) Serial.println();
}

//// arduiono utilities
void serialCon::doReset()
{
     endInstruction();
     if (debugMode) Serial.println(F("---- Resetting --- "));
     else Serial.println(F("reset"));
     debugMode = DEBUG_START;
          
}


void serialCon::doRestart()
{
  if (debugMode) Serial.println(F("---   Restarting  -----"));
  else Serial.println(F("restart"));
  delay(30);
  asm volatile ("  jmp 0");
}


void serialCon::checkMemory(bool force = false)
{
	int mem = freeMemory();
	if (mem < MEMORY_THRESH){
		if (debugMode) Serial.print(F("WARNING! LOW MEMORY: "));
		else Serial.println(F("memory"));
		force = true;
	}
	if (force){
		if (debugMode) Serial.println(mem);
	}
}

void serialCon::printInstructions(bool force)
{
	if (debugMode || force){
     Serial.println(F("q:Reboot   r:Reset/callibrate   m:Memory   d/t: Debug/terse  i:Id  h:Help  s:Status"));
	}
}



void serialCon::printStatus(){
  if (errorStr.length()>0) {
    Serial.print("errors:");
    Serial.println(errorStr);
  }
  else if (ready){
    Serial.println(F("ready"));
  }
  errorStr = "";
}

void serialCon::finishInit(){
  doReset();
  ready = true;
  printStatus();
  Serial.println(F("STARTING"));
}


///  Main loop processing
bool conReadData = false;

bool serialCon::processIncomingSerial(){
   conReadData = false;
  
  /// load one byte at a time
  while (Serial.available() >0) {
    if (Serial.available() >= SERIAL_RX_BUFFER_SIZE - 1){
      errorStr += F(" Serial Maxed");  
      printStatus();
    }
    conReadData = true;
    char inByte = Serial.read();       
    processEmergencyRestart(inByte);
    if (reading == false){
      /*if (inByte == '\r'){} //discard carriage return
      else if (inByte == '\n'){
        
        if (reading == false){
           if (debugMode) Serial.println(F("dummy new line"));
        }        
        else{
           endInstruction();          
        }        
      }
      */      
      if (inByte == 'q'){
        doRestart(); 
      }
      else if (inByte == 'r'){
        doReset();
      }                  
      else if (inByte == 'm'){
         checkMemory(true);
      }
      else if (inByte == 'd'){
          debugMode = true;
          if (debugMode)  Serial.println(F("Debug mode is ON"));                                  
      }
      else if (inByte == 't'){
          if (debugMode)  Serial.println(F("Debug mode is OFF"));   
          debugMode = false;                                              
      }
      else if (inByte == 'i'){
          sendID();                
      }
      else if (inByte == 'h'){
      	printInstructions(true);
      }
      else if (inByte == 's'){
        printStatus();
      }
      else{
      	reading = processMoreCommands(inByte);
      }
      
    }
    //// DATA ENTRY
    else{
       if (debugMode){
          inByte = shiftAsciiInput(inByte);
       }
       processMoreData(inByte);
    }  
  }
  
  return conReadData;
}

void serialCon::endLoop(bool _readData){
  loopsSinceMemCheck++;
  if (loopsSinceMemCheck > loopsBetweenMemCheck){
    loopsSinceMemCheck = 0;
    checkMemory();
  }
  
}

void serialCon::processEmergencyRestart(uint8_t inByte){
  bool match = true;
  if (inByte == restartString[matched]){
    matched++;
    if (matched==matchLength){
      Serial.println(F("Detected restart string"));
      doRestart();
    } 
  }
  else{
    matched = 0;
  }
  
}