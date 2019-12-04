//----------------------------------------------------
// Test EEPROM
//----------------------------------------------------

#include <EEPROM.h>

float locatie[][2] = { 
	{ 52.000111, 5.111111 },
	{ 52.000222, 5.222222 },
	{ 52.000333, 5.333333 },
	{ 52.000444, 5.444444 },
	{ 52.000555, 5.555555 },
	{ 52.000666, 5.666666 },
	{ 52.000777, 5.777777 },
	{ 52.000888, 5.888888 },
	{ 52.000999, 5.999999 },
	{ 53.000000, 5.000000 },
	{ 53.000111, 6.111111 },
	{ 53.000222, 6.222222 }
};
const byte locatieLength = sizeof(locatie) / sizeof(locatie[0]);
const byte passLength = 6;

char password[locatieLength][passLength] = { 
	"00001", "00002", "00003", "00004", "00005", "00006", "00007", "00008", "00009", "00010", "00011", "00012" 
};

int sizeCO, sizePass;

float valueF;
char valueC[passLength];

bool programmerMode = false;

void setup() 
{
	Serial.begin(9600);

	EEPROM_write();

	Serial.println("");
	Serial.println("");

	EEPROM_read();
	// EEPROM_reset();

	Serial.println("");
	Serial.println("Klaar!");
}

void loop() {}

void EEPROM_write() 
{
	for(byte i = 0; i < locatieLength; i++) 
	{
		sizeCO = i * (2*sizeof(float));

		EEPROM.put(sizeCO, locatie[i][0]);
		Serial.print(sizeCO);

		Serial.print("\t");

		EEPROM.put(sizeCO + 4, locatie[i][1]);
		Serial.println(sizeCO + 4);
	}

	Serial.println("");

	for(byte i = 0; i < locatieLength; i++) 
	{
		sizePass = i * (12*sizeof(char)) + sizeof(locatie);
		
		EEPROM.put(sizePass, password[i]);
		Serial.print(sizePass);
		Serial.print("\t");
	}
	Serial.println("");
}

void EEPROM_read() 
{
	for(byte i = 0; i < locatieLength; i++) 
	{
		sizeCO = i * (12*sizeof(float));

		Serial.print(EEPROM.get(sizeCO, valueF), 6);
		Serial.print("\t");
		Serial.println(EEPROM.get(sizeCO + 4, valueF), 6);
	}

	Serial.println("");

	for(byte i = 0; i < locatieLength; i++) 
	{
		sizePass = i * (12*sizeof(char)) + sizeof(locatie);
		
		Serial.print(sizePass);
		Serial.print("\t");
		Serial.print(EEPROM.get(sizePass, valueC));
		Serial.println("");
	}
	Serial.println("");
}

void EEPROM_reset() 
{
	for(int i = 0; i < EEPROM.length(); i++)
	{
		EEPROM.write(i, 0);
		Serial.println(EEPROM.get(i, valueF));
	}
}

