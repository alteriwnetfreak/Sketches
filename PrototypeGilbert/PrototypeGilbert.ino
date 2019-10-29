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
	{'1', '2', '3'},
	{'4', '5', '6'},
	{'7', '8', '9'},
	{'*', '0', '#'}
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
byte colPins[COLS] = {7, 8, 9}; 
byte rowPins[ROWS] = {10, 11, 12, 13}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Variables | Keypad
int dataCount = 0;
#define passwordLength 6

char data[passwordLength] = "";
char passWord[passwordLength] = "21199";
char passWordReset[passwordLength] = "2#111";
bool passwordBeingReset = false;


//*********************************************
// Include GPS
//*********************************************
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

SoftwareSerial serial_connection(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
TinyGPSPlus gps;

// Variables | GPS
// Posities om te testen LAT/LNG
float locatie1[] = { 52.024295, 5.554947 };
float locatie2[] = { 52.024705, 5.556341 };

float myLAT, myLNG;
int multiplier = 1000000;

//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
//const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; // Digital pins
const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



//*********************************************
// Setup
//*********************************************
void setup() {
  lcd.begin(16, 2);
  Serial.begin(2400);
  serial_connection.begin(9600);

  //lcd.print("Voer wachtwoord in: ");
  lcd.print("Testing GPS");
}


//*********************************************
// Loop
//*********************************************
void loop() {
	// GPS
		
	// GPS test
	while(serial_connection.available()) {
		gps.encode(serial_connection.read());
	}
	
	if(gps.location.isUpdated()) {
		myLAT = gps.location.lat();
		myLNG = gps.location.lng();

		lcd.clear();
		lcd.home();
		lcd.print((locatie1[0] / myLAT - 1) * multiplier);
		lcd.setCursor(0, 1);
		lcd.print((locatie1[1] / myLNG - 1) * multiplier);

		Serial.println("Satellite count: ");
		Serial.println(gps.satellites.value());
		Serial.println("Latitude: ");
		Serial.println(myLAT, 6);
		Serial.println("Longitude: ");
		Serial.println(myLNG, 6);
		// Serial.println("Speed MPH: ");
		// Serial.println(gps.speed.mph());
		// Serial.println("Altitude: ");
		// Serial.println(gps.altitude.feet());
		Serial.println("");
	}


	// // Keypad | LCD-scherm
	// giveData();

	// if(!passwordBeingReset) {
	// 	if(dataCount == passwordLength - 1) {
	// 		if(!strcmp(data, passWord)) {
	// 			//Serial.println("Correct!");
	// 			lcd.clear();
	// 			lcd.home();
	// 			lcd.print("Correct!");
	// 		} else if(!strcmp(data, passWordReset)) {
	// 			//Serial.println("New Pass: ");
	// 			lcd.clear();
	// 			lcd.home();
	// 			lcd.print("New Pass: ");

	// 			passwordBeingReset = !passwordBeingReset;
	// 		} else {
	// 			//Serial.println("Incorrect!");
	// 			//Serial.println("Try Again...");
	// 			lcd.clear();
	// 			lcd.home();
	// 			lcd.print("Incorrect!");
	// 			lcd.setCursor(0, 1);
	// 			lcd.print("Try again...");
	// 		}
	// 		clearData();
	// 	}
	// } else {
	// 	for(int i = 0; i < passwordLength - 1; i++) {
	// 		passWord[i] = data[i];
	// 	}
	// 	if(dataCount == passwordLength - 1) {
	// 		//Serial.print("New Pass: ");
	// 		//Serial.println(passWord);
	// 		lcd.home();
	// 		lcd.print("New Pass: ");
	// 		lcd.print(passWord);
	// 		lcd.print("!");
	// 		clearData();
	// 		passwordBeingReset = !passwordBeingReset;
	// 	}
	// }
}

// char* giveData() {
// 	char customKey = customKeypad.getKey();
// 	if(customKey) {
// 		data[dataCount] = customKey;
// 		dataCount++;
// 		//Serial.println(data);
// 		lcd.setCursor(0, 1);
// 		lcd.print(data);
// 	}
// 	return data;
// }

// void clearData() {
// 	for(int i = 0; i < passwordLength; i++) {
// 		data[i] = 0;
// 	}
// 	dataCount = 0;
// }