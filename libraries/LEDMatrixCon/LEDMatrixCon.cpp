#include "LEDMatrixCon.h"


LEDMatrixCon::LEDMatrixCon(Adafruit_DotStarMatrix* _matrix, LIBRARY _library, bool _zigzag, uint8_t w, uint8_t h, uint8_t* _borders) : LEDCon(_library)
{
	width = w;
	height = h;
	zigzag = _zigzag;
	borders = _borders;
	//leds_plus_safety_pixel = stringPlusSafety;
	//matrix = leds_plus_safety_pixel + 1;  // an alias for the visible real pixe
	matrix = _matrix;

	defaultColor = CRGB(0,0,0);	
}

void LEDMatrixCon::printInstructions(bool force){
	LEDCon::printInstructions(force);
	if (debugMode || force){
		Serial.println(F("x:Texture"));
	}
}

bool LEDMatrixCon::processMoreCommands(char inByte){
	LEDCon::processMoreCommands(inByte);	

	if (inByte == 'x'){
        mi=borders[0];
        mj=borders[1];
        mode= MATRIX;
        if (debugMode) Serial.println(F("Fill matrix"));         
    }
    
    if (mode == IDLE){
		return false;
	}
	return true;
}

void LEDMatrixCon::processMoreData(char inByte){	
	if (mode == MATRIX){                 
	     uint8_t color = inByte;
	     colorData[mk] = color;
	     mk++;                 
	     if (mk==3){
	        mk=0;
	        uint16_t color = getCorrectedColor();               
	        //int index = XY(mi, mj);	        
	        matrix->drawPixel(mi, mj, color);

	        mi++;
	     }
	     if (mi >= width - borders[2]){
	        mi=borders[0];
	        mj++;	        
	     }
	     if (mj >= height - borders[3]){	        
	        endInstruction();           
	     }
	}
	else{
		LEDCon::processMoreData(inByte);
	}
}

void LEDMatrixCon::fill()
{

	uint16_t color = getCorrectedColor();
    //matrix->fillScreen(color);  
    for (int i=borders[0]; i< width - borders[2]; i++){
    	for (int j=borders[1]; j< height - borders[3]; j++){
    		matrix->drawPixel(i,j,color);
    	}
    }
}

uint16_t LEDMatrixCon::getCorrectedColor(){
	// first white correct
	CRGB corrected;
	corrected.r = colorData.r * mWhite.r/255.;
	corrected.g = colorData.g * mWhite.g/255.;
	corrected.b = colorData.b * mWhite.b/255.;
	
	// now gamma 
	corrected = gammaCorrect(corrected);

	return matrix->Color(corrected.r, corrected.g, corrected.b);	
}


// helper functions
uint16_t LEDMatrixCon::XY( uint8_t x, uint8_t y){
	 uint16_t i;  
	  if( zigzag == false) {
	    i = (y * width) + x;
	  }
	  else if( zigzag == true) {
	    if( y & 0x01) {
	      // Odd rows run backwards
	      uint8_t reverseX = (width - 1) - x;
	      i = (y * width) + reverseX;
	    } else {
	      // Even rows run forwards
	      i = (y * width) + x;
	    }
	  }  
	  return i;
}

/*
uint16_t LEDMatrixCon::XYsafe( uint8_t x, uint8_t y){
	if( x >= width) return -1;
	if( y >= height) return -1;
	return XY(x,y);
}
*/