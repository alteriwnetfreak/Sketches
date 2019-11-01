//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

#include <Adafruit_NeoPixel.h>

#define PIN 3
#define N_LEDS 2

int r1, r2, g1; 
int g2, b1, b2 = 0;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(N_LEDS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
	strip.begin();

	strip.setPixelColor(0, 255, 0, 255);

	strip.show();

	delay(1000);

	strip.setPixelColor(0, 0, 0, 0);
}

void loop() {

}