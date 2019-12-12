//----------------------------------------------------
// Test GPS voor de Arduino
//----------------------------------------------------

// COÖRDINATEN CIRCLE: 52.024669, 5.5557331
// COÖRDINATEN MIJN HUIS: 51.9776302, 5.6566043

//*********************************************
// Include GPS
//*********************************************
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial ss(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
TinyGPSPlus gps;

// Include LCD
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() 
{
	Serial.begin(9600);
	ss.begin(9600);
	Serial.println("GPS Start!");

	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("GPS Start!");
}

void loop() {
	while(ss.available() > 0)
	{
		gps.encode(ss.read());

		// char c = ss.read();
		// Serial.write(c);
		// lcd.write(c);
	}
	
	if(gps.location.isUpdated())
	{
		float lat = gps.location.lat();
		float lng = gps.location.lng();

		Serial.print("Satellite count: ");
		Serial.println(gps.satellites.value());
		Serial.print("Latitude: ");
		Serial.println(lat, 6);
		Serial.print("Longitude: ");
		Serial.println(lng, 6);

		lcd.clear();
		lcd.home();
		lcd.print("LAT: ");
		lcd.print(lat, 6);
		lcd.setCursor(0, 1);
		lcd.print("LNG: ");
		lcd.print(lng, 6);
	}
}
