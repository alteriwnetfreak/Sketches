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


// Global Variables | Keypad
byte dataCount = 0;
#define passwordLength 6
#define latCOsize 10

char data[passwordLength] = "";
char passWord[passwordLength] = "21199";
char passWordReset[passwordLength] = "2#111";

bool passwordBeingReset = false;

char COdata[latCOsize] = "";
char latCO[latCOsize] = "52.123456";
double newLAT;

void setup(){
	Serial.begin(9600);
	Serial.println("Voer wachtwoord in: ");
}

void loop(){
	if(!passwordBeingReset) {
		giveData();
		if(dataCount == passwordLength - 1) {
			if(!strcmp(data, passWord)) {
				Serial.println("Correct!");
			} else if(!strcmp(data, passWordReset)) {
				Serial.print("Old LAT Coördinate: ");
				Serial.println(latCO);
				
				passwordBeingReset = !passwordBeingReset;
			} else {
				Serial.println("Incorrect!");
				Serial.println("Try Again...");
			}
			clearData();
		}
	} else {
		giveCoordinate();

		for(byte i = 0; i < latCOsize - 1; i++) {
			latCO[i] = COdata[i];
			
		}
		if(dataCount == latCOsize - 1) {
			Serial.print("New LAT Coördinate: ");
			Serial.println(latCO);
			
			newLAT = atof(latCO);

			Serial.println(newLAT, 6);
			
			clearData();
			passwordBeingReset = !passwordBeingReset;
		}
	}
}

char* giveData() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		data[dataCount] = customKey;
		dataCount++;
		Serial.println(data);
	}
	return data;
}
char* giveCoordinate() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		COdata[dataCount] = customKey;
		dataCount++;
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