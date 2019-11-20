//----------------------------------------------------
// Keypad test
//----------------------------------------------------

// Including keypad
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

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Global Variables | Keypad
byte dataCount = 0;
#define passwordLength 6
#define latCOamount 4
#define latCOsize 10

char data[passwordLength] = "";
char passWord[passwordLength] = "21199";
char programmerMode[passwordLength] = "2#111";

bool pmSwitch = false;

char COdata[latCOsize] = "";
char latCO[latCOamount][latCOsize] = {
	"52.123456",
	"52.234567",
	"52.345678",
	"52.456789",
};
double newLAT;

void setup(){
	Serial.begin(9600);
	lcd.begin(16, 2);

	lcd.clear();
	lcd.home();
	lcd.print("Voer wachtwoord in: ");
}

void loop(){
	if(!pmSwitch) {
		giveData();
		if(dataCount == passwordLength - 1) {
			if(!strcmp(data, passWord)) {
				lcd.clear();
				lcd.home();
				lcd.print("Correct!");
			} else if(!strcmp(data, programmerMode)) {
				lcd.clear();
				lcd.home();
				lcd.print("Old LAT:");
				for(byte i = 0; i < latCOamount; i++) {
					Serial.println(latCO[i]);
				}
				pmSwitch = !pmSwitch;
			} else {
				lcd.clear();
				lcd.home();
				lcd.print("Incorrect!");
			}
			clearData();
		}
	} else {
		giveCoordinate();

		for(byte i = 0; i < latCOsize - 1; i++) {
			latCO[latCOamount][i] = COdata[i];

			Serial.println(latCO[latCOamount][i]);
			Serial.println(COdata);
		}

		if(dataCount == latCOsize - 1) {
			newLAT = atof(latCO[latCOamount]);

			Serial.print("New LAT Coördinate: ");
			// Serial.println(latCO[latCOamount], 6);
			Serial.println(newLAT, 6);
			Serial.println("");
			for(byte i = 0; i < latCOamount; i++) {
				Serial.println(latCO[i]);
			}
			clearData();
			pmSwitch = !pmSwitch;
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
	}
	return data;
}
char* giveCoordinate() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		COdata[dataCount] = customKey;
		// Serial.println(latCO[latCOamount][dataCount]);
		dataCount++;
		// Serial.println(COdata);
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