#include<FastLED.h>

//// LED CONFIGURATION
////////////////////
#define LED_PIN_0     2
#define NUM_STRANDS   9
#define NUM_LEDS    100   //per strand
#define BRIGHTNESS  250
#define COLOR_ORDER RGB
#define LED_TYPE    UCS1903B

#include "LEDPalettesCon.h"

/// globals for this sketch
LEDPalettesCon* controller;
CRGB leds[NUM_STRANDS][NUM_LEDS];

/// Other configuration
const String idstring = "myID";

//// --- LED Management --- ////

/// LED setup utilities
void addLeds_forpin(int pin,  CRGB* led){
  if (pin==2) FastLED.addLeds<LED_TYPE, 2, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==3) FastLED.addLeds<LED_TYPE, 3, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==4) FastLED.addLeds<LED_TYPE, 4, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==5) FastLED.addLeds<LED_TYPE, 5, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==6) FastLED.addLeds<LED_TYPE, 6, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==7) FastLED.addLeds<LED_TYPE, 7, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==8) FastLED.addLeds<LED_TYPE, 8, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==9) FastLED.addLeds<LED_TYPE, 9, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==10) FastLED.addLeds<LED_TYPE, 10, COLOR_ORDER>(led, NUM_LEDS);
  if (pin==11) FastLED.addLeds<LED_TYPE, 11, COLOR_ORDER>(led, NUM_LEDS);
}



void setup() {
     CRGB* ledsp = leds[0];
     controller = new LEDPalettesCon(ledsp,  FASTLED, NUM_LEDS, NUM_STRANDS);
     controller->init();
      
     Serial.println();
     Serial.println(F(" Serial Data to Led Strands"));
     Serial.println(F(" Small Design Firm c. 2016"));
     Serial.println();
     controller->setID(idstring);
     controller->sendID();
   
   /// Setup LED environment
   for (int i=0; i< NUM_STRANDS; i++){     
      int pin = LED_PIN_0 + i;
      addLeds_forpin(pin, leds[i]);          
   }
   FastLED.setBrightness(  BRIGHTNESS );
   controller->setGamma(127);
   controller->setDefaultWhite(CRGB(255,178,170));
   controller->finishInit();

}

bool lastpos;
bool readData = false;

void loop() {
  
  bool readdata = controller->processIncomingSerial();
  controller->endLoop(readData);
}
