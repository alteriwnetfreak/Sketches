//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

#include <FastLED.h>

//Define
#define PIN A5
#define NUM_LEDS 6

CRGB leds[NUM_LEDS];

void setup() {
	//Initialize the leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);

	//Debug
	Serial.begin(9600);

	for(int x=0; x<NUM_LEDS; x++){
		writeLED('R', x);
	}
	FastLED.show();

	writeLED('R', 0);
	writeLED('R', 3);
	writeLED('B', 1);
	writeLED('B', 4);

	FastLED.show();
}

void loop() {}

void writeLED(char color, int led) {
	if (color == 'R'){ 
		leds[led] = CRGB::Red; 
	} else if (color == 'G') { 
		leds[led] = CRGB::Green; 
	} else if (color == 'B') { 
		leds[led] = CRGB::Blue; 
	} else if (color == ' ') { 
		leds[led] = CRGB::Black; 
	}
}