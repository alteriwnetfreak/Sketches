//----------------------------------------------------
// Test EEPROM
//----------------------------------------------------

#include <EEPROM.h>

float locatie[][2] = { 
	{ 52.025210, 5.555854 },
	{ 52.026478, 5.556792 },
	{ 52.025814, 5.557516 },
	{ 52.024656, 5.556728 },
	{ 52.123456, 5.969696 },
	{ 52.987655, 5.432121 },
};
const byte locatieLength = sizeof(locatie) / sizeof(locatie[0]);

int password[locatieLength] = { 1234, 2345, 3456, 4567, 6777, 2121 };
char passwordChar[5] = "1234";

int sizeCO, sizePass;

float valueF;
int valueI;

bool programmerMode = false;

void setup() {
	Serial.begin(9600);
	
	Serial.println(locatieLength);
	Serial.println(sizeof(locatie));
	Serial.println(passwordChar);
	Serial.println(sizeof(passwordChar));
	Serial.println(sizeof(char));
	Serial.println("");

	EEPROM_write();

	Serial.println("");
	Serial.println("");

	EEPROM_read();

	Serial.println("");
	Serial.println("Klaar!");
}

void loop() {}

void EEPROM_write() {
	for(byte i = 0; i < locatieLength; i++) {
		sizeCO = i * (2*sizeof(float));

		EEPROM.put(sizeCO, locatie[i][0]);
		EEPROM.put(sizeCO + 4, locatie[i][1]);
		Serial.print(sizeCO);
		Serial.print("\t");
		Serial.println(sizeCO + 4);
	}

	Serial.println("");

	for(byte i = 0; i < locatieLength; i++) {
		sizePass = i * (sizeof(int)) + sizeof(locatie);
		
		EEPROM.put(sizePass, password[i]);
		Serial.print(sizePass);
		Serial.print("\t");
	}
	Serial.println("");
}

void EEPROM_read() {
	for(byte i = 0; i < locatieLength; i++) {
		sizeCO = i * (2*sizeof(float));

		Serial.print(EEPROM.get(sizeCO, valueF), 6);
		Serial.print("\t");
		Serial.println(EEPROM.get(sizeCO + 4, valueF), 6);
	}

	Serial.println("");

	for(byte i = 0; i < locatieLength; i++) {
		sizePass = i * (sizeof(int)) + sizeof(locatie);
		
		Serial.print(EEPROM.get(sizePass, valueI));
		Serial.print("\t");
	}
	Serial.println("");
}

