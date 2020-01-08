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

// Include LCD
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int sat = 0;
float lat = 0;
float lng = 0;
float latD = 0;
float lngD = 0;
int disToDes = 0;

void setup() 
{
	Serial.begin(9600);
	// ss.begin(9600); // Nodig voor SoftwareSerial
	Serial.println("GPS Start!");

	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("GPS Start!");
}

void loop() {
	while(Serial.available() > 0)
	{
		gps.encode(Serial.read());

		// char c = Serial.read();
		// Serial.write(c);
	}
	
	if(gps.location.isUpdated())
	{
		sat = gps.satellites.value();
		lat = gps.location.lat();
		lng = gps.location.lng();
		latD = lat - 52.025519;
		lngD = lng - 5.557415;
		disToDes = sqrt(sq(latD) + sq(lngD * (latD / 90))) * 100000;

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
		Serial.println(disToDes, 6);

		lcd.clear();
		lcd.home();
		lcd.print("SAT: ");
		lcd.print(sat, 6);
		lcd.setCursor(0, 1);
		lcd.print("Distance: ");
		lcd.print(disToDes, 6);
	}
}


// #include <TinyGPS++.h>
// TinyGPSPlus gps;

// void loop()
// {
// // This sketch displays information every time a new sentence is correctly encoded.
// 	while (serial.available() > 0)
// 	{
// 		gps.encode(serial.read());
// 		if (gps.location.isUpdated())
// 		{
// 			// Latitude in degrees (double)
// 			Serial.print("Latitude= ");
// 			Serial.print(gps.location.lat(), 6);
// 			// Longitude in degrees (double)
// 			Serial.print(" Longitude= ");
// 			Serial.println(gps.location.lng(), 6);
// 		}
// 	}
// }



//
// Results:
// 		SS:
// 			$GPRMC,121938.00,V,,,,,,,080120,,,N*76
// 			$GPVTG,,,,,,,,,N*30
// 			$GPGGA,121938.00,,,,,0,00,99.99,,,,,,*66
// 			$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30
// 			$GPGSV,1,1,01,05,,,24*7B
// 			$GPGLL,,,,,121938.00,V,N*4A

//		HS:
// 			$GPRMC,122637.00,V,,,,,,,080120,,,N*75
// 			$GPVTG,,,,,,,,,N*30
// 			$GPGGA,122637.00,,,,,0,00,99.99,,,,,,*65
// 			$GPGSA,A,1,,,,,,,,,,,,,99.99,99.99,99.99*30
// 			$GPGLL,,,,,122637.00,V,N*49


