//----------------------------------------------------
// Test EEPROM
//----------------------------------------------------
/***
    eeprom_iteration example.

    A set of example snippets highlighting the
    simplest methods for traversing the EEPROM.

    Running this sketch is not necessary, this is
    simply highlighting certain programming methods.

    Written by Christopher Andrews 2015
    Released under MIT licence.
***/

#include <EEPROM.h>

float locatie[][2] = { 
	{ 52.025210, 5.555854 },
	{ 52.026478, 5.556792 },
	{ 52.025814, 5.557516 },
	{ 52.024656, 5.556728 },
	{ 52.023989, 5.556685 },
	{ 52.024649, 5.555698 },
	{ 01.123456, 0.123456 },
	{ 12.345678, 2.345678 },
	{ 23.456789, 3.456789 },
	{ 34.567890, 4.567890 }
};

void setup() {
	Serial.begin(9600);

	for(int i = 0; i < locatie[].length; i++) {
		EEPROM.put(i * 8, locatie[i][0]);
		Serial.println(EEPROM.get(0, value1), 7);
		
		Serial.println("");

		EEPROM.put(i * 8 + 4, locatie[i][1]);
		Serial.println(EEPROM.get(0, value1), 7);

		Serial.println("");
	}

	for(int o = 0; o < locatie[].length; o++) {
		EEPROM.get(o * sizeof(float))
	}
	
	Serial.println("Klaar!");


	// EEPROM.write(0, valToInt1);
	// Serial.println(EEPROM.read(0));
}

void loop() {}

