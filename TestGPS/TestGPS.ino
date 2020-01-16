//----------------------------------------------------
// Test GPS voor de Arduino
//----------------------------------------------------

// COÖRDINATEN CIRCLE: 52.024669, 5.5557331
// COÖRDINATEN MIJN HUIS: 51.9776302, 5.6566043

//*********************************************
// Include GPS
//*********************************************
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


// Include LCD
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Include FastLED
#include <FastLED.h>

#define PIN 6
#define NUM_LEDS 8
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
		latD = lat - 52.025519;
		lngD = lng - 5.557415;
		disToDes = sqrt(sq(latD) + sq(lngD)) * 65000;
		disToDesLCD = map(disToDes, 0, 600, 8, 6);

		Serial.print("Satellite count: ");
		Serial.print(sat);
		Serial.print("\tLatitude: ");
		Serial.print(lat, 6);
		Serial.print("\tLongitude: ");
		Serial.print(lng, 6);
		Serial.print("\tLat difference: ");
		Serial.print(latD, 6);
		Serial.print("\tLong difference: ");
		Serial.print(lngD, 6);
		Serial.print("\tDistance: ");
		Serial.print(disToDes);
		Serial.print(" ");
		Serial.println(disToDesLCD);

		lcd.clear();
		lcd.home();
		lcd.print("SAT: ");
		lcd.print(sat);
		lcd.setCursor(0, 1);
		lcd.print("Distance: ");
		lcd.print(disToDes);

		for(byte i = 0; i < NUM_LEDS; i++) {
			if(i < disToDesLCD) {
				leds[i].setRGB(0, 0, 0);
			} else {
				leds[i].setRGB(20, 0, 0);
			}
		}
		FastLED.show();
	}
}

/*
	Voor password:
	//__ 3 pogingen, na 3 pogingen 	-->		password fout, LED wordt rood
		 ga naar volgende punt

	//__ Meer LEDs toevoegen

	//__ Wachtwoord in plaats van knop voor phase
		 bij punt 6 en 12 code invoeren om verder te kunnen gaan met het spel

	//__ knop PMMode langer (15s)

	//__ bij te ver hellen 				--> 	"Danger!", 1x een punt eraf
	//__ na 4-5 seconden 				--> 	door gaan met het spel/rekenen

	met phase 3 (gyro game) naar punt lopen
	bij punt aangekomen 			--> 	moet timer aflopen, gyro activeren, naar ander punt (The Circle) lopen
	bij eindpunt "The Circle" 		--> 	timer stoppen
	Final score laten zien
*/