//----------------------------------------------------
// Test FastLED
//----------------------------------------------------
#include <FastLED.h>

#define PIN A5
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];


void setup() {
	//Initialize the x leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);

	//Debug
	Serial.begin(9600);
}

void loop()
{
	for(int x = 0; x < NUM_LEDS; x++){
		//writeLED(0, x);
		leds[x].setRGB(1, 0, 0);
	}
	// FastLED.show();
}

void writeLED(byte color, byte led)
{
	if(color == 1) { 		leds[led].setRGB(1, 0, 0); }
	else if(color == 2) { 	leds[led].setRGB(0, 255, 0); }
	else if(color == 3) { 	leds[led].setRGB(0, 0, 255); }
	else if(color == 0) { 	leds[led].setRGB(0, 100, 255); }
	FastLED.show();
}