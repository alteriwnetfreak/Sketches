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
	{ '*', '0', '#' }
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

// const int MPU = 0x68; // I2C address of the MPU6050 accelerometer

// int16_t AcX, AcY, AcZ;
// int axis = 0;
// int pitch = 0;
// int roll = 0;


//*********************************************
// Include EEPROM
//*********************************************
#include <EEPROM.h>

int sizeCO, sizePass;
float valueF;
char valueC[6];


//*********************************************
// Setup
//*********************************************
void setup() {
	// Initialize LCD | Serial | SS
	lcd.begin(16, 2);
	Serial.begin(2400);
	serial_connection.begin(9600);

	// Initialize FastLED
	// FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	// for(int x = 0; x < NUM_LEDS; x++){
	//     writeLED('R', x);
	// }
	// FastLED.show();

	// Print start of the game
	lcd.print("WELCOME!");
	lcd.setCursor(0, 1);
	lcd.print("Pls go outside");
	
	// Initialize interface to the MPU6050
	// Wire.begin();
	// setupMPU();

	// Wire.begin();
	// Wire.beginTransmission(MPU);
	// Wire.write(0x6B);
	// Wire.write(0);
	// Wire.endTransmission(true);
}

//*********************************************
// Loop
//*********************************************
void loop() {
	while(serial_connection.available()) {
		gps.encode(serial_connection.read());
	}
	
	if(!pmMode) {
		if(gps.location.isUpdated()) {
			myLAT = gps.location.lat();
			myLNG = gps.location.lng();
			distanceLAT = (myLAT / locatie[nextLocation][0] - 1) * multiplier; // multiplier hier(onder)
			distanceLNG = (myLNG / locatie[nextLocation][1] - 1) * multiplier;
			disToDes = sqrt(sq(distanceLAT) + sq(distanceLNG));

			// Serial.print(myLAT);
			// Serial.print("\t");
			// Serial.print(myLNG);
			// Serial.print("\t");
			// Serial.print(distanceLAT);
			// Serial.print("\t");
			// Serial.print(distanceLNG);
			// Serial.print("\t");
			// Serial.println(disToDes);

			// if-statement | andere optie: (distanceLAT < 0.15 && distanceLAT > -0.15) && (distanceLNG < 0.10 && distanceLNG > -0.10)
			if(disToDes < 100 && disToDes > -100 && nextLocation != sizeof(locatie) / sizeof(locatie[0])) {
				lcd.clear();
				lcd.home();
				lcd.print("Password: ");
				lcd.print(passWord[nextLocation]);
				Serial.println(passWord[nextLocation]);
				Serial.println(nextLocation);

				pmMode = !pmMode;
			} else {
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
	} else {
		giveData();
		if(!passwordBeingReset && dataCount == passwordLength - 1) {
			if(!strcmp(data, passWord[nextLocation])) {
				lcd.clear();
				lcd.home();
				lcd.print("Correct!");
				
				// writeLED('B', nextLocation);
				// FastLED.show();
				
				nextLocation++;
				pmMode = !pmMode;
			} else {
				lcd.clear();
				lcd.home();
				lcd.print("Incorrect!");
			}
			clearData();
		}
		if(nextLocation != 3) {
			giveData();
			if(!passwordBeingReset && dataCount == passwordLength - 1) {
				if(!strcmp(data, passWord[nextLocation])) {
					lcd.clear();
					lcd.home();
					lcd.print("Correct!");
					
					// writeLED('B', nextLocation);
					// FastLED.show();
					
					nextLocation++;
					pmMode = !pmMode;
				} 
				// else if(!strcmp(data, passWordReset)) {
				// 	lcd.clear();
				// 	lcd.home();
				// 	lcd.print("New Pass: ");

				// 	passwordBeingReset = !passwordBeingReset;
				// } 
				else {
					lcd.clear();
					lcd.home();
					lcd.print("Incorrect!");
				}
				clearData();
			} 
			// else {
			// 	givePassword();
			// }
		} else if(nextLocation == 3) {
			
		}
	}
}

// Functions Gyro
void setupMPU() {
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
	// 	pmMode = !pmMode;
	// }
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
		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);
	}
	for(byte o = 0; o < latlngAmount; o++) {
		sizePass = o * (12*sizeof(char)) + 100;
		memcpy(passWord[o], EEPROM.get(sizePass, valueC), sizeof(passWord[0]));
	}
}

// functions LEDs
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