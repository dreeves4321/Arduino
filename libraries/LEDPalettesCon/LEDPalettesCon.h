#ifndef LEDPALETTESCON_H
#define LEDPALETTESCON_H

#include "LEDCon.h"

#define MAX_PALETTES 9

class LEDPalettesCon :
public LEDCon
{
public:
	LEDPalettesCon(CRGB* _leds, LIBRARY _library, int _nLeds, int _nStrands);
	~LEDPalettesCon(){};

	virtual void doReset();
	virtual void printInstructions(bool force = false);	
	virtual bool processMoreCommands(char inByte);
	virtual void processMoreData(char inByte);

	virtual void fill();	
	virtual void show(){FastLED.show();}
	virtual void setBrightness(uint8_t _b){FastLED.setBrightness(_b);};
	
	// helper functions
	void	clearPalettes();
	int 	getPaletteID();
	void 	assignPalette(uint8_t pixel);
	void 	setPixelBrightness(uint8_t _b);

private:
	
	
public:

private:
	CRGB*   leds; // ALL of them
	int 	nLeds; // per strand
	int 	nStrands;

	CRGB	 palettes[MAX_PALETTES];
	uint8_t  nPalettes = 0;
	uint8_t  paletteWeights[MAX_PALETTES];
	bool	 uniform;

	float	 mPixelBrightnessMultiplier = 1.0;

};

#endif