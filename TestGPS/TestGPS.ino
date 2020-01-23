//----------------------------------------------------
// Test GPS voor de Arduino
//----------------------------------------------------

//*********************************************
// Include GPS
//*********************************************
#include <math.h>

#include <TinyGPS++.h>
// #include <SoftwareSerial.h> // Nodig voor SoftwareSerial

TinyGPSPlus gps;
// SoftwareSerial ss(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4 // Nodig voor SoftwareSerial

int sat = 0;
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
	// ss.begin(9600); // Nodig voor SoftwareSerial
	Serial.println("GPS Start!");

	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("GPS Start!");

	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	for(byte i = 0; i < NUM_LEDS; i++) {
		leds[i].setRGB(0, 0, 0);
	}
	FastLED.show();
}

void loop() {
	if(Serial.available() > 0)
	{
		char c = Serial.read();

		gps.encode(c);
		// Serial.write(c);
	}
	
	if(gps.location.isUpdated())
	{
		sat = gps.satellites.value();
		lat = gps.location.lat();
		lng = gps.location.lng();
		latD = lat - 52.024646;
		lngD = lng - 5.555738;
		disToDes = sqrt(sq(latD) + sq(lngD)) * 65000;
		disToDesLCD = map(disToDes, 0, 600, NUM_LEDS, NUM_LEDS * 0.75);
		direction = atan(latD / lngD);

		// Serial.print("Satellite count: ");
		// Serial.print(sat);
		// Serial.print("\tLatitude: ");
		// Serial.print(lat, 6);
		// Serial.print("\tLongitude: ");
		// Serial.print(lng, 6);
		// Serial.print("\tLat difference: ");
		// Serial.print(latD, 6);
		// Serial.print("\tLong difference: ");
		// Serial.print(lngD, 6);
		// Serial.print("\tDistance: ");
		// Serial.print(disToDes);
		// Serial.print(" ");
		// Serial.println(disToDesLCD);

		lcd.clear();
		lcd.home();
		lcd.print("Direction: ");
		lcd.print(direction);
		lcd.setCursor(0, 1);
		lcd.print("Distance: ");
		lcd.print(disToDes);

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