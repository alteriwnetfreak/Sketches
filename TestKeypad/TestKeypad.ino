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
#define passwordLength 6
#define latlngAmount 4
#define latCOsize 10
#define lngCOsize 9
byte dataCount = 0;

char data[passwordLength] = "";
char passWord[passwordLength] = "21199";
char programmerMode[latlngAmount][2][passwordLength] = { 
	{ "1#001", "2#001" },
	{ "1#002", "2#002" },
	{ "1#003", "2#003" },
	{ "1#004", "2#004" }
};

bool pmSwitch = false;
bool pmMode = false;

// Coordinaten Latitude
char COdata[latCOsize] = "";
float latlngCO[latlngAmount][2] = {
	{ 52.123456, 5.098765 },
	{ 52.234567, 5.987654 },
	{ 52.345678, 5.876543 },
	{ 52.456789, 5.765432 },
};
float newLAT;


// Include LCD
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Knop voor ProgrammerMode
long knopIngedrukt = 0;
int knopStatus = 0;



// SETUP
void setup() {
	Serial.begin(9600);
	pinMode(6, INPUT);
	lcd.begin(16, 2);

	lcd.clear();
	lcd.home();
	lcd.print("Voer wachtwoord in: ");

	Serial.println("Voer wachtwoord in: ");
}

// LOOP
void loop(){
	int knop = digitalRead(6);

	if(!pmMode) {
		if(knop != knopStatus) {
			knopStatus = knop;
			if(knop == 1) {
				knopIngedrukt = millis();
			} else {
				if(millis() - knopIngedrukt >= 10000) {
					Serial.println("We switchen naar programmerMode");
					Serial.println("Vul wachtwoord in:");

					lcd.clear();
					lcd.home();
					lcd.print("pmMode. geef ww:");
					pmSwitch = true;
				}
			}
		}

		giveData();
		if(!pmSwitch && dataCount == passwordLength - 1) {
			if(!strcmp(data, passWord)) {
				lcd.clear();
				lcd.home();
				lcd.print("Correct!");

				Serial.println("Correct!");
			} else {
				lcd.clear();
				lcd.home();
				lcd.print("Incorrect!");

				Serial.println("Incorrect!");
			}
			clearData();
		} else {
			if(dataCount == passwordLength - 1) {
				Serial.println(strtok(data, "#"));
				// if(!strcmp(data, programmerMode)) {
				// 	lcd.clear();
				// 	lcd.home();
				// 	lcd.print("Old LAT:");
				// 	lcd.setCursor(0, 1);
				// 	lcd.print(latlngCO[1][0], 6);

				// 	Serial.println("Old Coördinates: ");
				// 	for(byte i = 0; i < latlngAmount; i++) {
				// 		Serial.print("LAT: ");
				// 		Serial.print(latlngCO[i][0], 6);
				// 		Serial.print("\tLNG: ");
				// 		Serial.println(latlngCO[i][1], 6);
				// 	}
				// 	pmMode = !pmMode;
				// } else {
				// 	lcd.clear();
				// 	lcd.home();
				// 	lcd.print("Onjuist!");
				// 	Serial.println("Onjuist!!!!!!!!!!!");

				// 	pmSwitch = !pmSwitch;
				// }
				clearData();
			}
		}
	} else {
		giveCoordinate();
		if(dataCount == latCOsize - 1) {
			newLAT = atof(COdata);

			lcd.clear();
			lcd.home();
			lcd.print("New LAT:");
			lcd.setCursor(0, 1);
			lcd.print(newLAT, 6);

			Serial.print("New Coördinate: ");
			// Serial.println(latlngCO[latlngAmount], 6);
			Serial.println(newLAT, 6);
			Serial.println("");
			for(byte i = 0; i < latlngAmount; i++) {
				Serial.print("LAT: ");
				Serial.print(latlngCO[i][0], 6);
				Serial.print("\tLNG: ");
				Serial.println(latlngCO[i][1], 6);
			}
			clearData();
			pmMode = !pmMode;
		}
	}
}

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
/*
*/