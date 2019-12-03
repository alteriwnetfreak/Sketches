//----------------------------------------------------
// Prototype GilbertSpel
//----------------------------------------------------

//*********************************************
// Include Keypad
//*********************************************
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 3; 

char hexaKeys[ROWS][COLS] = {
	{ '1', '2', '3' },
	{ '4', '5', '6' },
	{ '7', '8', '9' },
	{ '.', '0', '#' }
};
//On keypad-board, from left to right
//Connect to pins: 7 to 13
//  -->    /////// /
//		 -------------
//		 | 1   2   3 |
//		 |           |
//		 | 4   5   6 |
//		 |           |
//		 | 7   8   9 |
//		 |           |
//		 | *   0   # |
//		 -------------
// // DIGITAL
// byte colPins[COLS] = { 7, 8, 9 };
// byte rowPins[ROWS] = { 10, 11, 12, 13 };
// "ANALOG":
byte colPins[COLS] = { 2, 5, 7 };
byte rowPins[ROWS] = { A3, A2, A1, A0 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Variables | Keypad
#define passwordLength 6 // min 1, wegens "closing char": (\n)
#define latlngAmount 12
#define latCOsize 10 // min 1, wegens "closing char": (\n)
#define lngCOsize 9 // min 1, wegens "closing char": (\n)
byte dataCount = 0;

char data[passwordLength] = "";
char passWord[latlngAmount][passwordLength];
byte passwordCorrect[12] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
char programmerMode[passwordLength] = "2#111";

// Coordinaten en Wachtwoord
char COdata[latCOsize] = "";
float latlngCO[latlngAmount][2];
float newCO;



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


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//*********************************************
// Include FastLED
//*********************************************
#include <FastLED.h>

#define PIN A5
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];


//*********************************************
// Include Gyroscoop
//*********************************************
// #include <Wire.h>

// long accelX, accelY, accelZ;
// float gForceX, gForceY, gForceZ;
// byte pitch, roll;


//*********************************************
// Include EEPROM
//*********************************************
#include <EEPROM.h>

int sizeCO, sizePass;
float valueF;
char valueC[6];


//*********************************************
// Include pmMode | knop | timer
//*********************************************
// Variabelen voor ProgrammerMode
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
float timerTillPause = 2700000;
float timerAfterPause = 3600000;
float timerTillEnd = 6300000;

// Macros
#define PRINT(var) Serial.print(#var ": "); Serial.print(var); Serial.print("\t");
#define PRINTLN(var) Serial.print(#var ": "); Serial.println(var);



//*********************************************
// Setup
//*********************************************
void setup() {
	// Initialize Serial | SS | LCD
	Serial.begin(9600);
	serial_connection.begin(9600);
	lcd.begin(16, 2);

	// Read EEPROM for coördinates en passwords
	EEPROM_read();

	// Button for ProgrammerMode
	pinMode(6, INPUT);

	// Initialize the leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);

	// Print start of the game
	lcd.print("WELCOME!");
	lcd.setCursor(0, 1);
	lcd.print("Pls go outside");
	
	// // Initialize interface to the MPU6050
	// Wire.begin();
	// setupMPU();
}

//*********************************************
// Loop
//*********************************************
void loop() {
	while(serial_connection.available()) 
	{
		gps.encode(serial_connection.read());
	}
	
	// Code for LED's, checking which has to turn what color
	for(byte i = 0; i < sizeof(passwordCorrect); i++) 
	{
		if(passwordCorrect[i] == 1) 
		{
			writeLED(2, i);
		} 
		else if(passwordCorrect[i] == 2) 
		{
			writeLED(3, i);
		} 
		else 
		{
			writeLED(1, i);
		}
	}

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
			if(millis() - knopIngedrukt >= 3000) 
			{
				knopIngedrukt = 0;
				if(pmSwitch == 0) // Go into pmMode
				{
					lcd.clear();
					lcd.home();
					lcd.print("pmMode. Pass:");
					pmSwitch = 1;
					onDestination = true;
				} 
				else if(pmSwitch >= 1) // Get out of pmMode
				{
					lcd.clear();
					lcd.home();
					lcd.print("Back to Game!");
					pmSwitch = 0;
					pmMode = false;
					onDestination = false;
					showOnLCD();
				}
			}
			else
			{
				PRINTLN(millis() - knopIngedrukt);
			}
		}
	}

	if(!pmMode) 
	{
		if(!onDestination) 
		{
			if(gps.location.isUpdated()) 
			{
				myLAT = gps.location.lat();
				myLNG = gps.location.lng();
				distanceLAT = (myLAT / latlngCO[nextLocation][0] - 1) * multiplier; // multiplier hier(onder)
				distanceLNG = (myLNG / latlngCO[nextLocation][1] - 1) * multiplier;
				disToDes = sqrt(sq(distanceLAT) + sq(distanceLNG));

				Serial.print(myLAT);
				Serial.print("\t");
				Serial.print(myLNG);
				Serial.print("\t");
				Serial.print(distanceLAT);
				Serial.print("\t");
				Serial.print(distanceLNG);
				Serial.print("\t");
				Serial.println(disToDes);

				// if-statement | andere optie: (distanceLAT < 0.15 && distanceLAT > -0.15) && (distanceLNG < 0.10 && distanceLNG > -0.10)
				if(disToDes < 100 && disToDes > -100 && nextLocation != sizeof(latlngCO) / sizeof(latlngCO[0])) 
				{
					lcd.clear();
					lcd.home();
					lcd.print("Password: ");
					lcd.print(passWord[nextLocation]);
					PRINTLN(nextLocation);
					onDestination = !onDestination;
				} 
				else 
				{
					lcd.clear();
					lcd.home();
					// lcd.print(distanceLAT, 3);
					// lcd.setCursor(8, 0);
					// lcd.print(distanceLNG, 3);
					lcd.print("Proximity:");
					lcd.setCursor(0, 1);
					lcd.print(disToDes, 6);
				}
			}
		} 
		else 
		{
			giveData();
			if(pmSwitch == 0) // pmSwitch = 0, Not in ProgrammerMode | Should be the game
			{
				if(dataCount == passwordLength - 1) 
				{
					lcd.clear();
					lcd.home();
					if(!strcmp(data, passWord[nextLocation])) // Password Correct
					{
						lcd.print("Correct!");
						nextLocation++;
						onDestination = !onDestination;
					} 
					else // Password Incorrect
					{
						lcd.print("Incorrect!");
					}
					clearData();
				}
			} 
			else if(pmSwitch == 1) //********** pmSwitch = 1, First stage of pmMode | logging in
			{
				if(dataCount == passwordLength - 1) 
				{
					lcd.clear();
					lcd.home();
					if(!strcmp(data, programmerMode)) // If pmMode pass is correct | Go on to next phase
					{
						lcd.print("Which CO/Pass?");
						pmSwitch = 2;
					} 
					else // If pmMode pass is incorrect | Go back to the game
					{
						lcd.print("Incorrect!");
						pmSwitch = 0;
						showOnLCD();
					}
					clearData();
				}
			} 
			else // pmSwitch = 2, Second stage of pmMode | Changing values
			{
				if(dataCount == 2) 
				{
					PRINTLN(data);

					COposition = atoi(data);
					PRINTLN(COposition);

					lcd.clear();
					lcd.home();
					if(COposition > latlngAmount) // If typed is not compatible with the list, try again
					{
						lcd.print("Not Possible!");
					} 
					else // Go on to changing the value
					{
						lcd.print("Give LAT: ");
						pmMode = !pmMode;
					}
					clearData();
				}
			}
		}
	} 
	else
	{
		giveCoordinate();
		if(stadium == 0) 
		{
			if(dataCount == latCOsize - 1) 
			{
				sizeCO = (COposition - 1) * (2*sizeof(float));

				PRINTLN(sizeCO);
				PRINTLN(COposition);

				EEPROM.put(sizeCO, atof(COdata));
				EEPROM_read();

				lcd.clear();
				lcd.home();
				lcd.print("New LAT:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COposition - 1][0], 6);

				stadium++;
				clearData();
				showOnLCD();
			}
		} 
		else if(stadium == 1) 
		{
			if(dataCount == lngCOsize - 1) 
			{
				sizeCO = (COposition - 1) * (2*sizeof(float)) + 4;

				PRINTLN(sizeCO);
				PRINTLN(COposition);

				EEPROM.put(sizeCO, atof(COdata));
				EEPROM_read();

				lcd.clear();
				lcd.home();
				lcd.print("New LONG:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COposition - 1][1], 6);

				stadium++;
				clearData();
				showOnLCD();
			}
		} 
		else 
		{
			if(dataCount == passwordLength - 1) 
			{
				sizePass = (COposition - 1) * (12*sizeof(char)) + 100;

				PRINTLN(sizePass);
				PRINTLN(COposition);

				EEPROM.put(sizePass, COdata);
				EEPROM_read();

				lcd.clear();
				lcd.home();
				lcd.print("New Pass:");
				lcd.setCursor(0, 1);
				lcd.print(passWord[COposition - 1]);

				stadium = 0;
				pmSwitch = 2;
				pmMode = !pmMode;
				clearData();
				showOnLCD();
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
		
		PRINTLN(data);
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

		PRINTLN(COdata);
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
	return;
}

// EEPROM Functions
void EEPROM_read() {
	for(byte i = 0; i < latlngAmount; i++) {
		sizeCO = i * (2*sizeof(float));
		latlngCO[i][0] = EEPROM.get(sizeCO, valueF);
		Serial.print(latlngCO[i][0], 6);
		Serial.print("\t");
		Serial.print(sizeCO);

		Serial.print("\t");
		Serial.print("\t");

		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);
		Serial.print(latlngCO[i][1], 6);
		Serial.print("\t");
		Serial.println(sizeCO + 4);
	}
	Serial.println("");
	for(byte o = 0; o < latlngAmount; o++) {
		sizePass = o * (12*sizeof(char)) + 100;
		memcpy(passWord[o], EEPROM.get(sizePass, valueC), sizeof(passWord[0]));
		PRINT(passWord[o]);
		PRINTLN(sizePass);
	}
}

// functions LEDs
void writeLED(byte color, int led) {
	if(color == 1) 
	{
		leds[led].setRGB(0, 100, 255);
	} 
	else if(color == 2) 
	{
		leds[led].setRGB(0, 255, 0);
	} 
	else if(color == 3) 
	{
		leds[led].setRGB(0, 0, 255);
	} 
	else if(color == 0) 
	{
		leds[led].setRGB(0, 0, 0);
	}
	FastLED.show();
	return;
}

// Function Timer
void showOnLCD() {
	rememberTime = millis();
	PRINT(rememberTime);

	while(millis() - rememberTime < 2000)
	{
		lcd.clear();
		lcd.home();
		if(!pmMode) {
			if(!onDestination) {
				//
			} else {
				if(pmSwitch == 0) {
					lcd.print("Incorrect!");
					lcd.setCursor(0, 1);
					lcd.print("Try again");

					delay(1500);

					lcd.clear();
				} else if(pmSwitch == 1) {
					lcd.print("Incorrect!");
					
					delay(2000);

					lcd.clear();
					lcd.home();
					lcd.print("Back to game!");
				} else {
					lcd.print("Choose another:");
				}
			}
		} else {
			if(stadium == 0) {
				lcd.print("Give LAT:");
			} else if(stadium == 1) {
				lcd.print("Give LNG: ");
			} else {
				lcd.print("Give PASS: ");
			}
		}
	}
	PRINTLN(rememberTime);
	return;
}

// // Functions Gyro
// void setupMPU() {
	// // Read the accelerometer data
	// Wire.beginTransmission(MPU);
	// Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
	// Wire.endTransmission(false);
	// Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
	
	// AcX = Wire.read() << 8 | Wire.read(); // X-axis value
	// AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
	// AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value

	// // Calculating the pitch (rotation around Y-axis) and roll (rotation around X-axis)
	// pitch = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
	// roll = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;

	// Serial.print("Pitch: ");
	// Serial.print(abs(pitch));
	// Serial.print(" deg");
	
	// Serial.print("\t\t");

	// Serial.print("Roll: ");
	// Serial.print(abs(roll));
	// Serial.println(" deg");

	// // Print naar LCD
	// lcd.clear();
	// lcd.home();
	// lcd.print("Keep straight");
	// lcd.setCursor(0, 1);
	// lcd.print("P: ");
	// lcd.print(pitch);
	// lcd.print(" | R: ");
	// lcd.print(roll);

	// if((pitch < 2 && pitch > -2) && (roll < 2 && roll > -2)) {
	// 	nextLocation++;
	// 	onDestination = !onDestination;
	// }
// }