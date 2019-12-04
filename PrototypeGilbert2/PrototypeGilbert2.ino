// //----------------------------------------------------
// // Prototype GilbertSpel
// //----------------------------------------------------

// //*********************************************
// // Include Keypad
// //*********************************************
// #include <Keypad.h>

// const byte ROWS = 4; 
// const byte COLS = 3; 

// char hexaKeys[ROWS][COLS] = {
// 	{ '1', '2', '3' },
// 	{ '4', '5', '6' },
// 	{ '7', '8', '9' },
// 	{ '.', '0', '#' }
// };
// //On keypad-board, from left to right
// //Connect to pins: 7 to 13
// //  -->    /////// /
// //		 -------------
// //		 | 1   2   3 |
// //		 |           |
// //		 | 4   5   6 |
// //		 |           |
// //		 | 7   8   9 |
// //		 |           |
// //		 | *   0   # |
// //		 -------------
// // // DIGITAL:
// // byte colPins[COLS] = { 7, 8, 9 };
// // byte rowPins[ROWS] = { 10, 11, 12, 13 };
// // "ANALOG":
// byte colPins[COLS] = { 2, 5, 7 };
// byte rowPins[ROWS] = { A3, A2, A1, A0 };

// Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// // Variables | Keypad
// #define passwordLength 6 // min 1, wegens "closing char": (\n)
#define latlngAmount 12
// #define latCOsize 10 // min 1, wegens "closing char": (\n)
// #define lngCOsize 9 // min 1, wegens "closing char": (\n)
// byte dataCount = 0;

// // Coordinaten en Wachtwoord
// char data[passwordLength] = "";
// char passWord[latlngAmount][passwordLength];
// byte passwordCorrect[latlngAmount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
// char programmerMode[passwordLength] = "2#111";

// char COdata[latCOsize] = "";
float latlngCO[latlngAmount][2];
// float newCO;



//*********************************************
// Include GPS
//*********************************************
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// pins op de breadboard, van links naar rechts: --->	GND | TX-pin | RX-pin | VCC
SoftwareSerial serial_connection(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
TinyGPSPlus gps;

// Variables | GPS
bool onDestination = false;
byte nextLocation = 0;

float myLAT, myLNG;
float distanceLAT, distanceLNG;
float disToDes;
double multiplier = 1000000;


// //*********************************************
// // Include LCD
// //*********************************************
// #include <LiquidCrystal.h>

// const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
// //const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// //*********************************************
// // Include FastLED
// //*********************************************
// #include <FastLED.h>

// #define PIN A5
// #define NUM_LEDS 6
// CRGB leds[NUM_LEDS];


// //*********************************************
// // Include Gyroscoop
// //*********************************************
// // #include <Wire.h>

// // long accelX, accelY, accelZ;
// // float gForceX, gForceY, gForceZ;
// // byte pitch, roll;


// //*********************************************
// // Include EEPROM
// //*********************************************
// #include <EEPROM.h>

// int sizeCO, sizePass;
// float valueF;
// char valueC[6];


// //*********************************************
// // Include pmMode | knop | timer
// //*********************************************
// // Variabelen voor ProgrammerMode
byte stadium = 0;
byte pmSwitch = 0;
bool pmMode = false;

// Knop
byte knop = 0;
long knopIngedrukt = 0;
int knopStatus = 0;

int COposition;

// Timers
unsigned long rememberTime = 0;
// unsigned long timerTillPause = 2700000;
// unsigned long timerAfterPause = 3600000;
// unsigned long timerTillEnd = 6300000;

// Macros
#define PRINT(var) Serial.print(#var ": "); Serial.print(var); Serial.print("\t")
#define PRINTLN(var) Serial.print(#var ": "); Serial.println(var)



void setup() 
{
	// Initialize Serial
	Serial.begin(9600);

	// Button for ProgrammerMode
	pinMode(6, INPUT);
}

void loop()
{
	// Code for button pressed, checking if button is pressed long enough
	knop = digitalRead(6);
	if(knop != knopStatus) 
	{
		knopStatus = knop;
		if(knop == 1) 
		{
			knopIngedrukt = millis();
		} 
		else 
		{
			if(millis() - knopIngedrukt > 1000) 
			{
				PRINT(millis() - knopIngedrukt);
				knopIngedrukt = 0;
				
				onDestination = !onDestination;
			}
		}
	}

	// Navigation through the whole program
	if(!pmMode) // In the game itself, playing it, or trying to get in pmMode
	{
		PRINT(pmMode);

		if(!onDestination) // In the game itself, still needing to go to the designated location
		{
			PRINT(onDestination);
		}
		else // In the game itself, you are at the designated location. You need a password to go on
		{
			PRINT(onDestination);

			if(pmSwitch == 0) // In the game itself, you're expected to give a password to go on
			{
				PRINT(pmSwitch);
			}
			else if(pmSwitch == 1) // You need a pass to go on to pmMode, it should be "2#111"
			{
				PRINT(pmSwitch);
			}
			else if(pmSwitch == 2) // You need to give a location you want to change the information of
			{
				PRINT(pmSwitch);
			}
		}
	}
	else // You are in pmMode, here you change the information you have chosen to change
	{
		PRINT(pmMode);

		if(stadium == 0) // 1st piece, the Latitude of the coördinate
		{
			PRINT(stadium);
		}
		else if(stadium == 1) // 2nd piece, the Longitude of the coördinate
		{
			PRINT(stadium);
		}
		else if(stadium == 2) // 3st piece, the Password of the coördinate
		{
			PRINT(stadium);
		}
	}
	Serial.println("");

}