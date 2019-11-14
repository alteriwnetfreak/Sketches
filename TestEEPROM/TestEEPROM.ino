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
	{ 34.567890, 4.567890 },
	{ 45.678901, 5.678901 },
	{ 56.789012, 6.789012 }
};
byte locatieLength = sizeof(locatie) / sizeof(locatie[0]);

int password[12] = { 0000, 1111, 2222, 3333, 4444, 5555, 6666, 7777, 8888, 9999, 1234, 6789 };


float valueF;
int valueI;
bool programmerMode = false;

void setup() {
	Serial.begin(9600);
	//Serial.println(locatieLength);

	EEPROM_write();

	Serial.println("");
	Serial.println("");

	EEPROM_read();

	Serial.println("Klaar!");
}

void loop() {
	// if(!programmerMode) {
	// 	delay(5000);
	// 	programmerMode = !programmerMode;
	// } else {
	// 	EEPROM_write();
	// 	EEPROM_read();
	// 	programmerMode = !programmerMode;
	// }
}

void EEPROM_write() {
	for(int i = 0; i < locatieLength; i++) {
		EEPROM.put(i * (2*sizeof(float)), locatie[i][0]);
		EEPROM.put(i * (2*sizeof(float)) + 4, locatie[i][1]);
		Serial.println(i * (2*sizeof(float)));
		Serial.println(i * (2*sizeof(float)) + 4);
	}

	for(int i = 0; i < locatieLength; i++) {
		EEPROM.put(i * (sizeof(int)) + 120, password[i]);
		Serial.println(i * (sizeof(int)) + 120);
	}
}

void EEPROM_read() {
	for(int i = 0; i < locatieLength; i++) {
		Serial.print(EEPROM.get(i * (2*sizeof(float)), valueF), 7);
		Serial.print("\t");
		Serial.println(EEPROM.get(i * (2*sizeof(float)) + 4, valueF), 7);
	}

	Serial.println("");

	for(int i = 0; i < locatieLength; i++) {
		Serial.println(EEPROM.get(i * sizeof(int) + 120, valueI));
	}
}

