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

SoftwareSerial serial_connection(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
TinyGPSPlus gps;


//*********************************************
// SINUS VARIABLES
//*********************************************
// float rad_nr = 10.0; // pi = 3.14159
// int rad_divider = 11;
// float radian = rad_nr/rad_divider;

// float lvler = 140.8;
// float amp;

// int potPin = 1; // Pin voor de PotentioMeter: A1
// float potVal = 0.0;


void setup() {
	Serial.begin(2400);
	serial_connection.begin(9600);
	Serial.println("GPS Start!");
}

void loop() {
	//*********************************************
	// GPS
	//*********************************************
	while(serial_connection.available()) {
		gps.encode(serial_connection.read());
	}
	
	if(gps.location.isUpdated()) {
		Serial.print("Satellite count: ");
		Serial.println(gps.satellites.value());
		Serial.print("Latitude: ");
		Serial.println(gps.location.lat(), 6);
		Serial.print("Longitude: ");
		Serial.println(gps.location.lng(), 6);
		Serial.print("Speed MPH: ");
		Serial.println(gps.speed.mph());
		Serial.print("Altitude: ");
		Serial.println(gps.altitude.feet());
		Serial.println("");
	}
	


	//*********************************************
	// SINUS
	//*********************************************
	// potVal = analogRead(potPin);
	// amp = potVal / lvler;
	// //Serial.println(potVal);
	
	// Serial.println(amp*sin(radian));
	
	// radian = radian + rad_nr/rad_divider;
	// if(radian >= rad_nr) {
	// 	radian = rad_nr/rad_divider;
	// }

	//Serial.println("");
}
