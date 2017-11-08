#include "LEDCon.h"

/// Inherited
void LEDCon::doReset(){
	  colorData = defaultColor;
    setWhite(defaultWhite);
    fill();
    
    serialCon::doReset();
}

void LEDCon::printInstructions(bool force){
	serialCon::printInstructions(force);
	if (debugMode || force){
		Serial.println(F("f:Fill    b:Brightness     w:White   g: Gamma"));
	}
}

void LEDCon::endInstruction(){
	mi = 0; mj = 0; mk = 0;
  if (!suppressShow)  showFlag = true;
  suppressShow = false;
	mode = IDLE;
	serialCon::endInstruction();
}

bool LEDCon::processMoreCommands(char inByte){	
	  if (inByte == 'f'){
       mode = FILL;
       mk=0;
       if (debugMode) Serial.println(F("Fill started"));
    }           
    else if (inByte == 'b'){
       mode = BRIGHT;
       if (debugMode) Serial.println(F("Set brightness"));
    }           
    else if (inByte == 'w'){
       mode = WHITE;
       if (debugMode) Serial.println(F("Set white"));               
    }
    else if (inByte == 'g'){
       mode = GAMMA;
       if (debugMode) Serial.println(F("Set gamma"));
    }
    else if (inByte == ']'){       
       if (debugMode) Serial.println(F("dither on"));
       setDither(true);            
    }
    else if (inByte == '['){
       if (debugMode) Serial.println(F("dither off"));
       setDither(false);  
    }

    if (mode == IDLE){
		return false;
	}
	return true;
}

void LEDCon::processMoreData(char inByte){	
	if (mode == FILL){
          uint8_t color = inByte;
          colorData[mk] = color;
          mk++;
          if (mk==3){
            fill();
            if (debugMode){
              Serial.println(F("filled with "));
              Serial.println(getRGBString(colorData));
            } 
            endInstruction();
          }
     }
     else if (mode == BRIGHT){
          uint8_t bright = inByte;
          setBrightness(bright);                                                     
          endInstruction();
     }
      else if (mode == GAMMA){
          uint8_t g = inByte;
          setGamma(g);          
          endInstruction();
     }
     else if (mode == WHITE){
         uint8_t color = inByte;
         colorData[mk] = color;
         mk++;
         if (mk==3){
            setWhite(colorData);                
            endInstruction();
         }
     }     
}

/// LED control
void LEDCon::setDither(bool _on){
  if (mLibrary == FASTLED){
    if (_on) FastLED.setDither(BINARY_DITHER);
    else FastLED.setDither(DISABLE_DITHER);
  }
}

String LEDCon::getRGBString(CRGB _col){
	String rgb = String(_col.r) + " "+ String(_col.g) + " " + String(_col.b);
	return rgb;
};

void LEDCon::setWhite(CRGB _white){
	  if (mLibrary == FASTLED){
      FastLED.setCorrection(_white);
    }
    else{
      mWhite = _white;      
    }
    colorData = defaultColor;
    fill();

    if (debugMode){
      if (mLibrary == FASTLED){
        Serial.print(F("FASTLED white: "));
      }
      else{
        Serial.print(F("Manual white: "));
      }
	    
    	Serial.println(getRGBString(_white));
    }
};

void LEDCon::setDefaultWhite(CRGB _white){
	defaultWhite = _white;
	/*
	CRGB w = LEDColorCorrection::TypicalSMD5050;
	#ifdef TYPE_UCS1903B
		w = CRGB(255,200,200);
	#endif
	#ifdef TYPE_APA102
		w = CRGB(200,255,100);
	#endif
	*/
};


void LEDCon::fill(){
	Serial.println("LEDCon::fill");
};

void LEDCon::endLoop(bool _readData){
	if (showFlag == true && Serial.available() < 2){
    show();
    showFlag = false;
  }
  serialCon::endLoop();  
}

void LEDCon::setBrightness(uint8_t _b){
    if (mLibrary == FASTLED){
      FastLED.setBrightness(_b);
    }
    else{
      // must be done elsewhere
    }
}

void LEDCon::setGamma(uint8_t _b){
  mGamma = _b;
  if (mGamma <= 0) mGamma = 255;
  if (mGamma > 255) mGamma = 255;
  if (debugMode){
    Serial.print(F("Inverse Gamma = "));
    Serial.println(mGamma);
  }          
}

void LEDCon::setLowCutoff(uint8_t _low){
  mLowCutoff = _low;
}

CRGB LEDCon::gammaCorrect(CRGB _in, float _brightnessparam){

  if (mGamma >= 255) return _in;
  if (mGamma <= 0) return _in;
  
  if (debugMode){
    //Serial.println("uncorrected color " + getRGBString(_in));
  }


  // stretch bottom down    
  int r = _in.r*(255. + mLowCutoff)/255. - mLowCutoff;
  int g = _in.g*(255. + mLowCutoff)/255. - mLowCutoff;
  int b = _in.b*(255. + mLowCutoff)/255. - mLowCutoff;
  
  if (debugMode){
    //Serial.println("stretched colors " + String(r) + " " + String(g) + " " + String(b));
  }

  // apply extra boost
  if (_brightnessparam != 1.0){
    r *= _brightnessparam;
    g *= _brightnessparam;
    b *= _brightnessparam;
  }

  // apply exponent
  float a = 255./mGamma;
  _in.r = 255.*pow(r/255., a);
  _in.g = 255.*pow(g/255., a);
  _in.b = 255.*pow(b/255., a);
  

  if (debugMode){
    Serial.println("corrected color " + getRGBString(_in));
  }

  return _in;
}