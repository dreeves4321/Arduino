#include <Adafruit_DotStarMatrix.h>

//// LED CONFIGURATION
////////////////////
#define DATAPIN  51 //4
#define CLOCKPIN 53 //5
#define COLOR_ORDER DOTSTAR_GBR
//#define CHIPSET     APA102
#define WIDTH 20
#define HEIGHT 20
#define BORDERX0 1     // blank border that is NOT filled or addressed from serial data
#define BORDERY0 2
#define BORDERX1 3
#define BORDERY1 4
#define NUM_LEDS (WIDTH * (HEIGHT - BORDERY1) )  // don't bother making the last border row. The rest need to be made and will simply never be filled
#define BRIGHTNESS  250


#include "LEDMatrixCon.h"

/// Globals
LEDMatrixCon* controller;
//CRGB leds_plus_safety_pixel[NUM_LEDS + 1];

/// Other configuration
//const bool    kMatrixSerpentineLayout = true;
const String idstring = "myID";
 
void setup() {
    Adafruit_DotStarMatrix* matrix = new Adafruit_DotStarMatrix( WIDTH, HEIGHT, 
    DS_MATRIX_TOP     + DS_MATRIX_RIGHT +
    DS_MATRIX_COLUMNS + DS_MATRIX_ZIGZAG,
    COLOR_ORDER);
 
    matrix->begin();
    matrix->setBrightness(BRIGHTNESS);

    uint8_t borders[4] = {BORDERX0, BORDERY0, BORDERX1, BORDERY1};
    controller = new LEDMatrixCon(matrix, ADAFRUIT, true, WIDTH, HEIGHT, borders);
    controller->init();

    Serial.println();
    Serial.println(F("   LED matrix controller from serial"));
    Serial.println(F("   Small Design Firm c. 2016"));
    Serial.println();
    controller->setID(idstring);
    controller->sendID();
    
    
    /// Setup LED environment
   controller->setDefaultWhite(CRGB(197,250,173));
   controller->setBrightness(BRIGHTNESS);
   controller->finishInit();
   controller->fill();
   controller->show();
}


bool readData = false;

void loop() {
   
  readData = controller->processIncomingSerial();  
  controller->endLoop(readData);

  
}
