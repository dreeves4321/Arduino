#include "LEDPalettesCon.h"

LEDPalettesCon::LEDPalettesCon(CRGB* _leds, LIBRARY _library, int _nLeds, int _nStrands) : LEDCon(_library){
	leds = _leds;
	nLeds= _nLeds;
	nStrands = _nStrands;
};


void LEDPalettesCon::doReset(){
	clearPalettes();
  
	LEDCon::doReset();	
}

void LEDPalettesCon::printInstructions(bool force){
	 LEDCon::printInstructions(force);
	 if (debugMode || force) Serial.println(F("a:Add palette   p:assign Pixel   u:Uniform  v:Varying   c:clear palettes  e:extra brightness"));
};	

bool LEDPalettesCon::processMoreCommands(char inByte){
	LEDCon::processMoreCommands(inByte);
	  if (inByte == 'c'){
        clearPalettes();               
    }
    else if (inByte == 'a'){
       mode = ADDPALETTE;
       nPalettes++;
       if (nPalettes > MAX_PALETTES) nPalettes = 0;
       mk=0;
       if (debugMode) Serial.println(F("Palette started"));
    }
    else if (inByte == 'e'){
      mode = BOOST;
      if (debugMode) Serial.println(F("Set extra brightness"));
    }
    else if (inByte == 'p'){
       mode = PIXEL;
       if (debugMode) Serial.println(F("Set pixel"));
    }
    else if (inByte == 'u'){
       uniform = true;
       if (debugMode){
         Serial.print(F("Uniform coloring is: ON"));         
       }
    }
    else if (inByte == 'v'){
       uniform = false;
       if (debugMode){
         Serial.print(F("Uniform coloring is: OFF"));   
       }
    }
    if (mode == IDLE){
		return false;
	}
	return true;

};

void  LEDPalettesCon::processMoreData(char inByte){
	LEDCon::processMoreData(inByte);
	if (mode == ADDPALETTE){
          uint8_t color = inByte;
          if (mk<3){
             colorData[mk] = color;
          }
          else{
            paletteWeights[nPalettes-1] = color;
          }
          mk++;
          if (mk==4){
            palettes[nPalettes-1] = colorData;
            int z = int(nPalettes-1);

            if (debugMode) Serial.println("added Palette index " + String(z) + ": " + getRGBString(palettes[z])+ " " + paletteWeights[z]);
            suppressShow = true;
            endInstruction();
          }
     }
     else if (mode == PIXEL){
          uint8_t pixel = inByte;
          assignPalette(pixel);
          if (debugMode){
            Serial.print(F("Pixel set: ")); 
            int z = int(pixel);             
            Serial.println(String(z));
          }          
          endInstruction();
     }
     else if (mode == BOOST){
          uint8_t boost = inByte;
          mPixelBrightnessMultiplier = float(boost) / 255.;
          if (debugMode){
            Serial.print(F("Pixel Brightness Multiplier: "));             
            Serial.println(mPixelBrightnessMultiplier);
          }
          endInstruction();
     }
}

void  LEDPalettesCon::fill(){
	int numleds = nStrands * nLeds;
    CRGB corrected = gammaCorrect(colorData);
    for (int i=0; i< numleds; i++){
       leds[i] = corrected;
    }    
}	


void	 LEDPalettesCon::clearPalettes(){
   nPalettes = 0;
   if (debugMode) Serial.println(F("Palettes Cleared"));
}

int 	 LEDPalettesCon::getPaletteID(){
	int paletteInd = 0;
    if (nPalettes == 0){}
    else if (uniform){
        int maxInd = 0;
        int maxW = 0;
        for (int i=0; i<nPalettes; i++){
            if (paletteWeights[i]>maxW){
               maxW = paletteWeights[i];
               maxInd = i;
            }           
        }
        paletteInd = maxInd;
    }
    else{
       int sumW = 0;
       for (int i=0; i<nPalettes; i++) sumW += paletteWeights[i];
       int target = random(sumW);
     
        sumW = paletteWeights[0];
        while(sumW <= target){
           paletteInd++;
           sumW += paletteWeights[paletteInd]; 
     	}
    }
    return paletteInd;
}


void 	 LEDPalettesCon::assignPalette(uint8_t _pixel){
	if (nPalettes == 0){
    Serial.println("No palettes");
    return;  
  } 
    int paletteId = 0;
    for (int i=0; i<nStrands; i++){
       paletteId = getPaletteID();

       //Serial.println(paletteId);       
       CRGB corrected  = gammaCorrect(palettes[paletteId], mPixelBrightnessMultiplier);       
       leds[i*nLeds + _pixel] = corrected;
       //leds[i*nLeds + _pixel] = palettes[paletteId];
    }  
}
