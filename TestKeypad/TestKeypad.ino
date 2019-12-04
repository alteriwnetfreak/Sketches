//----------------------------------------------------
// Keypad test
//----------------------------------------------------

// Including keypad
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 3; 

char numPad[ROWS][COLS] = {
	{ '1', '2', '3' },
	{ '4', '5', '6' },
	{ '7', '8', '9' },
	{ '.', '0', '#' }
};
//On keypad-board, from left to right
//Connect to pins: 7 to 13
//   -->      /////// /
//          -------------
//          | 1   2   3 |
//          |           |
//          | 4   5   6 |
//          |           |
//          | 7   8   9 |
//          |           |
//          | *   0   # |
//          -------------
// // DIGITAL
// byte colPins[COLS] = { 7, 8, 9 };
// byte rowPins[ROWS] = { 10, 11, 12, 13 };
// // "ANALOG":
byte colPins[COLS] = { 2, 5, 7 };
byte rowPins[ROWS] = { A3, A2, A1, A0 };

Keypad customKeypad = Keypad(makeKeymap(numPad), rowPins, colPins, ROWS, COLS);


// Global Variables | Keypad
#define passwordLength 6 // min 1, wegens "closing char": (\n)
#define latlngAmount 12
#define latCOsize 10 // min 1, wegens "closing char": (\n)
#define lngCOsize 9 // min 1, wegens "closing char": (\n)
byte dataCount = 0;

char data[passwordLength] = "";
char passWord[latlngAmount][passwordLength];
char programmerMode[passwordLength] = "2#111";

// Coordinaten
char COdata[latCOsize] = "";
float latlngCO[latlngAmount][2];
float newCO;
int COposition;

// VARS GPS
byte nextLocation = 0;


// Include LCD
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Include EEPROM
#include <EEPROM.h>

int sizeCO, sizePass;
float valueF;
char valueC[6];


// Include FastLED
#include <FastLED.h>

#define PIN A5
#define NUM_LEDS 6

CRGB leds[NUM_LEDS];


// Variabelen voor ProgrammerMode
bool pmMode = false;
byte pmSwitch = 0;
byte pmState = 0;

// Knop
long knopIngedrukt = 0;
int knopStatus = 0;


// SETUP
void setup() 
{
	Serial.begin(9600);
	// Read EEPROM for coördinates en passwords
	EEPROM_read();
	
	// Button for ProgrammerMode
	pinMode(6, INPUT);

	// Initialize the leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	
	// Initialize LCD
	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("Voer wachtwoord in: ");

	Serial.println("Voer wachtwoord in: ");
}

// LOOP
void loop()
{
	int knop = digitalRead(6);

	// Code for button pressed, checking if button is pressed long enough
	if(knop != knopStatus) 
	{
		knopStatus = knop;
		if(knop == 1) 
		{
			knopIngedrukt = millis();
		} 
		else 
		{
			if(millis() - knopIngedrukt >= 1000) 
			{
				knopIngedrukt = 0;
				if(pmSwitch == 0) // Go into pmMode
				{
					lcd.clear();
					lcd.home();
					lcd.print("pmMode. geef ww:");
					
					Serial.println("We switchen naar programmerMode");
					Serial.println("Vul wachtwoord in:");
					
					pmSwitch = 1;
				} 
				else if(pmSwitch >= 1) // Get out of pmMode
				{
					lcd.clear();
					lcd.home();
					lcd.print("Terug naar spel!");

					Serial.println("En terug naar het spel!");

					pmSwitch = 0;
					pmMode = false;
				}
			}
		}
	}

	// Code for LED's, checking which has to turn what color
	for(byte i = 0; i < NUM_LEDS; i++) 
	{
		if(3 + pmSwitch == i) {
			writeLED('G', i);
		} else {
			writeLED('B', i);
		}
	}

	// Main code for going through ProgrammerMode, changing value's en testing purposes
	if(!pmMode) // pmMode off
	{
		giveData();
		if(pmSwitch == 0) // pmSwitch = 0, Not in ProgrammerMode | Should be the game
		{
			if(dataCount == passwordLength - 1)
			{
				if(!strcmp(data, passWord[nextLocation])) // Password Correct
				{
					lcd.clear();
					lcd.home();
					lcd.print("Correct!");

					writeLED('G', 0);
					nextLocation++;

					Serial.println("Correct!");
				} 
				else // Password Incorrect
				{
					lcd.clear();
					lcd.home();
					lcd.print("Incorrect!");

					writeLED('B', 0);

					Serial.println("Incorrect!");
				}
				clearData();
			}
		} 
		else if(pmSwitch == 1) // pmSwitch = 1, First stage of pmMode | logging in
		{
			if(dataCount == passwordLength - 1)
			{
				if(!strcmp(data, programmerMode)) // If pmMode pass is correct | Go on to next phase
				{
					Serial.println("Old Coördinates: ");
					for(byte i = 0; i < latlngAmount; i++) 
					{
						Serial.print("LAT ");
						Serial.print(i + 1);
						Serial.print(":\t");
						Serial.print(latlngCO[i][0], 6);
						Serial.print("\tLNG ");
						Serial.print(i + 1);
						Serial.print(":\t");
						Serial.print(latlngCO[i][1], 6);
						Serial.print("\tPass ");
						Serial.print(i + 1);
						Serial.print(": ");
						Serial.println(passWord[i]);
					}
					Serial.println("Welke positie wil je veranderen?: ");

					lcd.clear();
					lcd.home();
					lcd.print("Welke positie?");

					pmSwitch++;
				} 
				else // If pmMode pass is incorrect | Go back to the game
				{
					lcd.clear();
					lcd.home();
					lcd.print("Onjuist!");
					
					Serial.println("Onjuist!!!!!!!!!!!");
					pmSwitch = 0;
				}
				clearData();
			}
		} 
		else // pmSwitch = 2, Second stage of pmMode | Changing values
		{
			if(dataCount == 2)
			{
				COposition = atoi(data);
			    Serial.print("Positie: ");
			    Serial.println(COposition);

				// If typed is not compatible with the list, try again
				if(COposition == 0 || COposition > latlngAmount) 
				{
					lcd.clear();
				    lcd.home();
				    lcd.print("Niet Geldig!");

				    Serial.println("Niet Geldig!");
				} 
				else // Go on to changing the value
				{
					lcd.clear();
					lcd.home();
					lcd.print("Geef Latitude:");

					Serial.println("Geef Latitude: ");
					
					pmMode = !pmMode;
				}
				clearData();
			}
		}
	} 
	else // pmMode on
	{
		giveCoordinate();
		if(pmState == 0) // Change LAT
		{
			if(dataCount == latCOsize - 1) 
			{
				sizeCO = (COposition - 1) * (2*sizeof(float));
				EEPROM.put(sizeCO, atof(COdata));
				Serial.println("Geef Longitude:");

				lcd.clear();
				lcd.home();
				lcd.print("New LAT:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COposition - 1][0], 6);

				clearData();
				pmState++;
			}
		} 
		else if(pmState == 1) // Change LONG
		{
			if(dataCount == lngCOsize - 1) 
			{
				sizeCO = (COposition - 1) * (2*sizeof(float)) + 4;
				EEPROM.put(sizeCO, atof(COdata));
				Serial.println("Geef password:");

				lcd.clear();
				lcd.home();
				lcd.print("New LONG:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COposition - 1][1], 6);

				clearData();
				pmState++;
			}
		} 
		else // Change PASS
		{
			if(dataCount == passwordLength - 1) 
			{
				sizePass = (COposition - 1) * (12*sizeof(char)) + sizeof(latlngCO);
				EEPROM.put(sizePass, COdata);
				EEPROM_read();

				lcd.clear();
				lcd.home();
				lcd.print("New Pass:");
				lcd.setCursor(0, 1);
				lcd.print(passWord[COposition - 1]);

				clearData();
				pmMode = !pmMode;
				pmSwitch = 2;
				pmState = 0;
			}
		}
	}
}

// Functions Keypad
char* giveData() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		data[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(data);
		Serial.println(data);
	}
	return data;
}
char* giveCoordinate() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		COdata[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(COdata);
		Serial.println(COdata);
	}
	return COdata;
}
void clearData() {
	for(byte i = 0; i < passwordLength; i++) {
		data[i] = 0;
	}
	for(byte i = 0; i < latCOsize; i++) {
		COdata[i] = 0;
	}
	dataCount = 0;
}

// EEPROM Functions
void EEPROM_read() {
	for(byte i = 0; i < latlngAmount; i++) {
		sizeCO = i * (2*sizeof(float));

		latlngCO[i][0] = EEPROM.get(sizeCO, valueF);

		Serial.print(sizeof(latlngCO[i]) / 2);
		Serial.print("\t");
		Serial.print(sizeCO);
		Serial.print("\t");
		Serial.print(valueF, 6);
		Serial.print("\t\t\t");


		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);

		Serial.print(sizeof(latlngCO[i]) / 2);
		Serial.print("\t");
		Serial.print(sizeCO + 4);
		Serial.print("\t");
		Serial.println(valueF, 6);
	}
	Serial.println("");

	for(byte o = 0; o < latlngAmount; o++) {
		sizePass = o * (12*sizeof(char)) + sizeof(latlngCO);
		
		memcpy(passWord[o], EEPROM.get(sizePass, valueC), sizeof(passWord[0]));

		Serial.print(sizePass);
		Serial.print("\t");
		Serial.print(o * (12*sizeof(char)));
		Serial.print("\t");
		Serial.println(valueC);

	}
	Serial.println("");
	Serial.println("");
}

// FastLED Functions
void writeLED(char color, int led) {
	if (color == 'R'){ 
		leds[led] = CRGB::Red; 
	} else if (color == 'G') { 
		leds[led] = CRGB::Green; 
	} else if (color == 'B') { 
		leds[led] = CRGB::Blue; 
	} else if (color == ' ') { 
		leds[led] = CRGB::Black; 
	}
	FastLED.show();
}
