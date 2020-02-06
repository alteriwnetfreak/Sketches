//----------------------------------------------------
// Test GPS voor de Arduino
//----------------------------------------------------

// Include GPS
#include <TinyGPS++.h>

TinyGPSPlus gps;

byte sat = 0;
float lat = 0;
float lng = 0;
float latD = 0;
float lngD = 0;
int disToDes = 0;
int disToDesLCD = 0;
float direction = 0;


// Include LCD
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Include FastLED
#include <FastLED.h>

#define PIN 6
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];


void setup() 
{
	Serial.begin(9600);
	Serial.println("GPS Starting...");

	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("GPS Starting...");

	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	for(byte i = 0; i < NUM_LEDS; i++) 
	{ 
		leds[i].setRGB(0, 0, 0); 
	}
	FastLED.show();
}

void loop()
{
	while(Serial.available() > 0)
	{
		// char c = Serial.read();

		gps.encode(Serial.read());
		// Serial.write(c);
	}
	
	if(gps.location.isUpdated())
	{
		sat = gps.satellites.value();
		lat = gps.location.lat();
		lng = gps.location.lng();
		latD = lat - 52.025502;
		lngD = (lng - 5.556829) * (lat / 90.0);
		disToDes = sqrt(sq(latD) + sq(lngD)) * 65000;
		// disToDesLCD = map(disToDes, 0, 600, NUM_LEDS, NUM_LEDS * 0.75);
		direction = latD / lngD;

		lcd.clear();
		lcd.home();
		if(direction > 0.414 || direction < -0.414)
		{
			if(latD < 0) { lcd.print("N"); }
			if(latD > 0) { lcd.print("S"); }
		}
		lcd.setCursor(1, 0);
		lcd.print("/");
		if(direction < 2.414 && direction > -2.414)
		{
			if(lngD < 0) { lcd.print("E"); }
			if(lngD > 0) { lcd.print("W"); }
		}

		// Serial.print("Satellite count: ");
		// Serial.print(sat);
		Serial.print("Latitude: ");
		Serial.print(lat, 6);
		Serial.print("\tLongitude: ");
		Serial.print(lng, 6);
		Serial.print("\tLat difference: ");
		Serial.print(latD, 6);
		Serial.print("\tLong difference: ");
		Serial.print(lngD, 6);
		// Serial.print("\tDistance: ");
		// Serial.print(disToDes);
		// Serial.print(" | ");
		// Serial.print(disToDesLCD);
		Serial.print("\tDirection: ");
		Serial.print(direction, 6);
		Serial.println("");

		// lcd.clear();
		// lcd.home();
		// lcd.print("Distance: ");
		// lcd.print(disToDes);
		lcd.setCursor(0, 1);
		lcd.print("Direction: ");
		lcd.print(direction);

		// for(byte i = 0; i < NUM_LEDS; i++) {
		// 	if(i < disToDesLCD) {
		// 		leds[i].setRGB(0, 0, 0);
		// 	} else {
		// 		leds[i].setRGB(20, 0, 0);
		// 	}
		// }
		// FastLED.show();
	}
}