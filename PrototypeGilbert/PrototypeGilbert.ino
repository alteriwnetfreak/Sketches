//----------------------------------------------------
// PrototypeGilbert
//----------------------------------------------------
//*********************************************
// Including keypad
//*********************************************
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

char data[passwordLength] = "";
char COdata[latCOsize] = "";
byte dataCount = 0;

char programmerMode[passwordLength] = "2#111";
byte COposition;


//*********************************************
// Include GPS | Software Serial
//*********************************************
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial serial_connection(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
TinyGPSPlus gps;
byte nextLocation = 0;
bool onDestination = false;
float LATDifference, LONGDifference;
int disToDes;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//*********************************************
// Include EEPROM
//*********************************************
#include <EEPROM.h>

int sizeCO, sizePass;
float valueF;
char valueC[6];
float latlngCO[latlngAmount][2];
char passWord[latlngAmount][passwordLength];
byte passwordCorrect[latlngAmount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


//*********************************************
// Include FastLED
//*********************************************
#include <FastLED.h>

#define PIN A5
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];


// Variabelen voor ProgrammerMode
bool pmMode = false;
byte pmSwitch = 0;
byte pmState = 0;
bool youHaveWonTheGame = false;

// Knop
long knopIngedrukt = 0;
byte knopStatus = 0;

//Timers
long rememberTime = 0;


// SETUP
void setup() 
{
	// Initialize Serial | SS
	Serial.begin(9600);
	serial_connection.begin(9600);

	// Read EEPROM for coördinates en passwords
	EEPROM_read();
	
	// // Button for ProgrammerMode
	// pinMode(6, INPUT);

	// Initialize the leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	
	// Initialize LCD
	lcd.begin(16, 2);
	lcd.clear();
}

// LOOP
void loop()
{
	while(serial_connection.available()){
		gps.encode(serial_connection.read());
	}

	// // Code for button pressed, checking if button is pressed long enough
	// byte knop = digitalRead(6);
	// if(knop != knopStatus) 
	// {
	// 	knopStatus = knop;
	// 	if(knop == 1) 
	// 	{
	// 		knopIngedrukt = millis();
	// 	} 
	// 	else 
	// 	{
	// 		if(millis() - knopIngedrukt >= 5000) 
	// 		{
	// 			rememberTime = millis();
	// 			knopIngedrukt = 0;
	// 			lcd.clear();
	// 			lcd.home();
	// 			while(millis() - rememberTime < 1000)
	// 			{
	// 				if(pmSwitch == 0) // Go into pmMode
	// 				{
	// 					lcd.print("PMMode pass: ");
	// 					pmSwitch = 1;
	// 				} 
	// 				else if(pmSwitch >= 1) // Get out of pmMode
	// 				{
	// 					lcd.print("Back to game!");
	// 					pmSwitch = 0;
	// 					pmMode = false;
	// 				}
	// 			}
	// 			lcd.clear();
	// 		}
	// 	}
	// }

	// Code for LED's, checking which has to turn what color
	for(byte i = 0; i < NUM_LEDS; i++) 
	{
		if(youHaveWonTheGame)
		{
			writeLED(2, i);
		}
		else
		{
			if(passwordCorrect[i] == 1) {
				writeLED(2, i);
			} else {
				writeLED(0, i);
			}
		}
	}

	// Main code for going through ProgrammerMode, changing value's en testing purposes
	if(!youHaveWonTheGame)
	{
		if(!pmMode) // pmMode off
		{
			giveData();
			if(pmSwitch == 0) // pmSwitch = 0, Not in ProgrammerMode | Should be the game
			{
				if(gps.location.isUpdated()) // Every time the GPS get's a new location
				{
					// Code for Coordinates from GPS
					LATDifference = gps.location.lat() - latlngCO[nextLocation][0];
					LONGDifference = gps.location.lng() - latlngCO[nextLocation][1];
					disToDes = sqrt(sq(LATDifference) + sq(LONGDifference));
					Serial.print("Latitude: ");
					Serial.print(LATDifference); 
					Serial.print("\t");
					Serial.print("Longitude: ");
					Serial.print(LONGDifference);
					Serial.print("\t");
					Serial.print("Distance: ");
					Serial.println(disToDes);

					if(nextLocation < latlngAmount)
					{
						if(onDestination)
						{
							lcd.home();
							lcd.print("Password: ");
							if(dataCount == passwordLength - 1)
							{
								if(!strcmp(data, passWord[nextLocation])) // Password Correct | Go on to next point
								{
									showOnLCD();
									passwordCorrect[nextLocation] = 1;
									nextLocation++;
									Serial.println(nextLocation);
									onDestination = false;
								} 
								else // Password Incorrect | try again
								{
									showOnLCD();
								}
								clearData();
							}
						}
						else // Not at the desired location | show disToDes on LCD
						{
							lcd.home();
							lcd.print("Distance:");
							lcd.setCursor(0, 1);
							lcd.print(disToDes);

							if(disToDes < 10)
							{
								onDestination = true;
							}
						}
					}
					else // You have been to all the locations/points || time is up
					{
						youHaveWonTheGame = true;
					}
				}
				// else // When the GPS does not get a new location
				// {
				// 	clearData();
				// 	// lcd.clear();
				// 	lcd.home();
				// 	lcd.print("SEARCHING FOR:");
				// 	lcd.setCursor(0, 1);
				// 	lcd.print("Connection");
				// }
			} 
			else if(pmSwitch == 1) // pmSwitch = 1, First stage of pmMode | logging in
			{
				lcd.home();
				lcd.print("PMMode pass: ");
				if(dataCount == passwordLength - 1)
				{
					if(!strcmp(data, programmerMode)) // If pmMode pass is correct | Go on to next phase
					{
						showOnLCD();
						Serial.println("Old Coördinates: ");
						EEPROM_read();
						pmSwitch++;
					} 
					else // If pmMode pass is incorrect | Go back to the game
					{
						showOnLCD();
						pmSwitch = 0;
					}
					clearData();
				}
			} 
			else // pmSwitch = 2, Second stage of pmMode | Changing values
			{
				lcd.home();
				lcd.print("Which location?");
				if(dataCount == 2)
				{
					COposition = atoi(data);
				    Serial.print("Positie: ");
				    Serial.println(COposition);

					// If typed is not compatible with the list, try again
					if(COposition == 0 || COposition > latlngAmount) 
					{
					    showOnLCD();
					} 
					else // Go on to changing the value
					{
						showOnLCD();
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
				lcd.home();
				lcd.print("LAT: ");
				if(dataCount == latCOsize - 1) 
				{
					sizeCO = (COposition - 1) * (2*sizeof(float));
					EEPROM.put(sizeCO, atof(COdata));
					EEPROM_read();
					showOnLCD();
					clearData();
					pmState++;
				}
			} 
			else if(pmState == 1) // Change LONG
			{
				lcd.home();
				lcd.print("LONG: ");
				if(dataCount == lngCOsize - 1) 
				{
					sizeCO = (COposition - 1) * (2*sizeof(float)) + 4;
					EEPROM.put(sizeCO, atof(COdata));
					EEPROM_read();
					showOnLCD();
					clearData();
					pmState++;
				}
			} 
			else // Change PASS
			{
				lcd.home();
				lcd.print("Pass: ");
				if(dataCount == passwordLength - 1) 
				{
					sizePass = (COposition - 1) * (12*sizeof(char)) + sizeof(latlngCO);
					EEPROM.put(sizePass, COdata);
					EEPROM_read();
					showOnLCD();
					clearData();
					pmMode = !pmMode;
					pmSwitch = 2;
					pmState = 0;
				}
			}
		}
	}
	else
	{
		pmMode = false;
		pmSwitch = 0;
		pmState = 0;
		showOnLCD();
	}
}

// Functions Keypad
char* giveData() 
{
	char customKey = customKeypad.getKey();
	if(customKey) 
	{
		data[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(data);
		Serial.println(data);
	}
	return data;
}
char* giveCoordinate() 
{
	char customKey = customKeypad.getKey();
	if(customKey) 
	{
		COdata[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(COdata);
		Serial.println(COdata);
	}
	return COdata;
}
void clearData() 
{
	for(byte i = 0; i < passwordLength; i++) 
	{
		data[i] = 0;
	}
	for(byte i = 0; i < latCOsize; i++) 
	{
		COdata[i] = 0;
	}
	dataCount = 0;
}

// EEPROM Functions
void EEPROM_read() 
{
	for(byte i = 0; i < latlngAmount; i++) 
	{
		sizeCO = i * (2*sizeof(float));
		sizePass = i * (12*sizeof(char)) + sizeof(latlngCO);

		latlngCO[i][0] = EEPROM.get(sizeCO, valueF);
		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);
		memcpy(passWord[i], EEPROM.get(sizePass, valueC), sizeof(passWord[0]));
		
		Serial.print(latlngCO[i][0], 6);
		Serial.print("\t");
		Serial.print(latlngCO[i][1], 6);
		Serial.print("\t");
		Serial.println(passWord[i]);
	}
	Serial.println("");
}

// FastLED Functions
void writeLED(byte color, byte led) 
{
	if (color == 1){ 
		leds[led] = CRGB::Red; 
	} else if (color == 2) { 
		leds[led] = CRGB::Green; 
	} else if (color == 3) { 
		leds[led] = CRGB::Blue; 
	} else if (color == 0) { 
		leds[led] = CRGB::Black; 
	}
	FastLED.show();
}

// LCD Functions
void showOnLCD()
{
	rememberTime = millis();
	lcd.clear();
	while(millis() - rememberTime < 1300) {
		lcd.home();
		if(!youHaveWonTheGame) {
			if(!pmMode) {
				if(pmSwitch == 0) {
					// if(gps.location.isUpdated()) {
						if(nextLocation < latlngAmount) {
							// if(onDestination) {
								if(!strcmp(data, passWord[nextLocation])) {
									lcd.print("Correct!");
								} else {
									lcd.print("Incorrect!");
								}
							// } else {
								//
							// }
						} else {
							lcd.print("You won! Go back");
							lcd.setCursor(0, 1);
							lcd.print(" to The Circle");
						}
					// } else {
					// 	// 
					// }
				} else if(pmSwitch == 1) {
					if(!strcmp(data, programmerMode)) {
						lcd.print("Which location?");
					} else {
						lcd.print("Onjuist!");
					}
				} else {
					if(COposition == 0 || COposition > latlngAmount) {
					    lcd.print("Niet Geldig!");
					} else
					{
						lcd.print("Geef Latitude:");
					}
				}
			} else {
				if(pmState == 0) {
					lcd.print("LAT: ");
					lcd.setCursor(0, 1);
					lcd.print(latlngCO[COposition - 1][0], 6);
				} else if(pmState == 1) {
					lcd.print("LONG: ");
					lcd.setCursor(0, 1);
					lcd.print(latlngCO[COposition - 1][1], 6);
				} else {
					lcd.print("Pass: ");
					lcd.setCursor(0, 1);
					lcd.print(passWord[COposition - 1]);
				}
			}
		} else {
			lcd.print("You won! Go back");
			lcd.setCursor(0, 1);
			lcd.print(" to The Circle!");
		}
	}
	lcd.clear();
	lcd.home();
	return;
}