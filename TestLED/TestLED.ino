//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

#include <FastLED.h>

//Define
#define PIN A5
#define NUM_LEDS 6

CRGB leds[NUM_LEDS];

void setup() {
  //Initialize the 3 leds
  FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);

  //Debug
  Serial.begin(9600);
}

void loop() {
	for(int x=0; x<NUM_LEDS; x++){
		writeLED('R', x);
	}
}

void writeLED(char color, int led) {
	if (color == 'R') {			leds[led] = CRGB::Red; } 
	else if (color == 'G') { 	leds[led] = CRGB::Green; } 
	else if (color == 'B') { 	leds[led] = CRGB::Blue; } 
	else if (color == ' ') { 	leds[led] = CRGB::Black; }
	FastLED.show();
}