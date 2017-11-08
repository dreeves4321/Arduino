#ifndef LEDMATRIXCON_H
#define LEDMATRIXCON_H

#include "LEDCon.h"
#include <Adafruit_DotStarMatrix.h>


class LEDMatrixCon :
public LEDCon
{
public:
	LEDMatrixCon(Adafruit_DotStarMatrix* _matrix, LIBRARY _library, bool _zigzag, uint8_t w, uint8_t h, uint8_t* borders);
	~LEDMatrixCon(){};

	virtual void printInstructions(bool force = false);	
	virtual bool processMoreCommands(char inByte);
	virtual void processMoreData(char inByte);

	virtual void fill();	
	virtual void show(){matrix->show();}
	virtual void setBrightness(uint8_t _b){matrix->setBrightness(_b);};

protected:
	uint16_t getCorrectedColor();
	// helper functions
	uint16_t XY( uint8_t x, uint8_t y);
	//uint16_t XYsafe( uint8_t x, uint8_t y);



public:
	Adafruit_DotStarMatrix* matrix;  // an alias for the visible real pixels

private:
	//Adafruit_DotStarMatrix leds_plus_safety_pixel; // the whole set plus one safety pixel at index=0

	uint8_t width;
	uint8_t height;
	bool zigzag = true;
	uint8_t* borders;

};

#endif