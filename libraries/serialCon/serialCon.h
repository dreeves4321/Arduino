////
// Arduino control over serial
// c. Small Design Firm 2016
//
//  Processes a reading loop every idle, reading and processing from the serial buffer. Base class supports some basic functions in the main loop, processIncomingSerial():   restart, reset, memory check, debug mode, send ID, status, instructions.
//
//  "reading" mode. If reading=True, then the regular functions are skipped, and processMoreData() is called. Presumably, the program is waiting for more data after a previous command started a read.
//
//  Extend this class by adding more commands via processMoreCommands(), adding "modes" for data reading, and defining processMoreData(). EndLoop() is also a good place to add more functionality.
//
//  Emergency restart string: if the Arduino receives this "secret code" it restarts.
//  Sensitive to maxing the serial buffer. Sends a message "Serial Maxed".
/////

#ifndef SERIALCON_H
#define SERIALCON_H

#include <Arduino.h>
#include <MemoryFree.h>

#define SERIAL_SPEED 115200
#define MEMORY_THRESH 100  //send warning if remaining memory is less
#define DEBUG_START false  // if in debug mode, print more data to serial console. Also shift ASCII input so keyboard covers all 256

class serialCon{
public:
	serialCon(){};
	~serialCon(){};

/// Serial Functions
	void init();
	void setID(String _string);
	void sendID();
	uint8_t shiftAsciiInput(char c);


//// arduiono utilities
	virtual void doReset();
	void doRestart();
	void checkMemory(bool force = false);
	
/// Status and printing
	virtual void printInstructions(bool force = false);	
	void setError(String _str){errorStr = _str;}
	void printStatus();
	void finishInit();

///  Main loop processing
	virtual void endInstruction();
	virtual bool processMoreCommands(char inByte){return false;};
	virtual void processMoreData(char inByte){};
	bool processIncomingSerial();
	virtual void endLoop(bool _readData = false);

private:
	void processEmergencyRestart(uint8_t c);

public:
	bool reading = false;
	bool debugMode = DEBUG_START;
	bool ready = false;
	String errorStr = "";	

protected:
	String mID = "";
	int loopsBetweenMemCheck = 1000;
	int loopsSinceMemCheck = 0;

private:
	const uint8_t*	restartString =  "12345"; //use characters that will never be instructions
	const int 		matchLength = 5;
	int 			matched = 0;
};
#endif SERIALCON_H