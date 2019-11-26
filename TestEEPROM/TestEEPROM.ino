//----------------------------------------------------
// Test EEPROM
//----------------------------------------------------

#include <EEPROM.h>

float locatie[][2] = { 
	{ 52.025210, 5.555854 },
	{ 51.026478, 5.556792 },
	{ 52.025814, 4.557516 },
	{ 52.024656, 5.556728 },
	{ 44.123456, 3.969696 },
	{ 52.987655, 5.432121 },
	{ 52.025210, 6.555854 },
	{ 12.026478, 5.556792 },
	{ 52.025814, 7.557516 },
	{ 52.024656, 5.556728 },
	{ 52.123456, 5.001696 },
	{ 52.987655, 5.432121 },
};
const byte locatieLength = sizeof(locatie) / sizeof(locatie[0]);

char password[locatieLength][6] = { 
	"11111",
	"11112",
	"11113",
	"11114",
	"11115",
	"11116",
	"11117",
	"11118",
	"11119",
	"11120",
	"11121",
	"11122"
};
char randomValue[6];

int sizeCO, sizePass;

float valueF;
char valueC[6];

void setup() {
	Serial.begin(9600);
	
	// Serial.println(locatieLength);
	// Serial.println(sizeof(locatie));
	// Serial.println(passwordChar);
	// Serial.println(sizeof(password));
	// Serial.println(sizeof(char));
	// Serial.println("");

	// EEPROM_write();

	// Serial.println("");
	// Serial.println("");

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
		sizePass = i * (7*sizeof(char)) + 100;
		
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
		sizePass = i * (7*sizeof(char)) + 100;
		
		memcpy(randomValue, EEPROM.get(sizePass, valueC), sizeof(randomValue));
		Serial.print(randomValue);
		Serial.print("\t");
	}
	Serial.println("");
}

