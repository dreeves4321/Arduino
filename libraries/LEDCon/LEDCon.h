////
// LED control over serial
// c. Small Design Firm 2016
//
// - extends serialCon to apply generic control over serial to controlling LEDs
// - use with LEDMatrixCon to control LED matrices, or with LEDPaletteCon to control strands with palettes.
// - to extend this class, you may want to add more modes to the enum. Theres definitely a more elegant way to
// do it, but it worked here. Then you will need to define show(), setBrightness(), and fill().
//
//
/////


#ifndef LEDCON_H
#define LEDCON_H

#include <FastLED.h>
#include "serialCon.h"
#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>

enum MODE {IDLE, FILL, ADDPALETTE, PIXEL, BRIGHT, WHITE, MATRIX, GAMMA, BOOST};
enum LIBRARY {FASTLED, ADAFRUIT};

class LEDCon :
public serialCon
{
public:
	LEDCon(LIBRARY _library){
		mLibrary = _library;		
	};
	~LEDCon(){};

	/// Inherited - be sure to call the base class when extending!
	virtual void doReset();
	virtual void printInstructions(bool force = false);
	virtual void endInstruction();
	virtual bool processMoreCommands(char inByte);
	virtual void processMoreData(char inByte);
	virtual void endLoop(bool readData = false);
	
	// implemented by library
	virtual void show(){};	
	virtual void setBrightness(uint8_t _b);
	
	/// LED control
	virtual void fill();	
	String getRGBString(CRGB _col);
	CRGB gammaCorrect(CRGB _inColor, float _brightnessparam = 1.0);

	void setWhite(CRGB _white);
	void setDefaultWhite(CRGB _white);
	void setDither(bool _on);
	void setGamma(uint8_t _gamma);
	void setLowCutoff(uint8_t _low);

	
public:	
	CRGB colorData;
	MODE mode = IDLE;

protected:
	LIBRARY mLibrary;
	uint8_t mGamma = 255;  // really inverse gamma.  applied as exponent = 255./Gamma
	uint8_t mLowCutoff = 10; // any color below this goes to zero when gamma corrected.

	uint8_t mi=0;
	uint8_t mj=0;
	uint8_t mk = 0;

	CRGB defaultColor = CRGB(125,125,125);	
	CRGB defaultWhite = LEDColorCorrection::TypicalLEDStrip;
	CRGB mWhite = CRGB(255,255,255);

	bool	showFlag = false;
	bool	suppressShow = false;
};

#endif