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
byte colPins[COLS] = {7, 8, 9}; 
byte rowPins[ROWS] = {10, 11, 12, 13}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Variables | Keypad
int dataCount = 0;
#define passwordLength 6

char data[passwordLength] = "";
char passWord[][passwordLength] = { "21199", "69666", "21420", "11111", "11*22", "22#33" };
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
bool onDestination = false;

float locatie[][2] = { 
	{ 52.025210, 5.555854 },
	{ 52.026478, 5.556792 },
	{ 52.025814, 5.557516 },
	{ 52.024656, 5.556728 },
	{ 52.023989, 5.556685 },
	{ 52.024649, 5.555698 }
};
int nextLocation = 0;

float myLAT, myLNG;
float distanceLAT, distanceLNG;
float disToDes;
int multiplier = 10000000;


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
// Include FastLED
//*********************************************
#include <FastLED.h>

#define PIN 6
#define NUM_LEDS 1

CRGB leds[NUM_LEDS];



//*********************************************
// Setup
//*********************************************
uint32_t recordedTime = 0;

void setup() {
	lcd.begin(16, 2);
	Serial.begin(2400);
	serial_connection.begin(9600);
	
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	for(int x=0; x<NUM_LEDS; x++){
	    writeLED('R', x);
	}
	FastLED.show();

	lcd.print("Setting up GPS..");
	lcd.setCursor(0, 1);
	lcd.print("Pls go outside");
}


//*********************************************
// Loop
//*********************************************
void loop() {
	// GPS
	while(serial_connection.available()) {
		gps.encode(serial_connection.read());
	}
	
	if(!onDestination) {
		if(gps.location.isUpdated()) {
			myLAT = gps.location.lat();
			myLNG = gps.location.lng();
			distanceLAT = (myLAT / locatie[nextLocation][0] - 1) * multiplier;
			distanceLNG = (myLNG / locatie[nextLocation][1] - 1) * multiplier;
			disToDes = sqrt(sq(distanceLAT) + sq(distanceLNG));
			
			// if-statement | andere optie: (distanceLAT < 0.15 && distanceLAT > -0.15) && (distanceLNG < 0.10 && distanceLNG > -0.10)
			if(disToDes < 0.2 && disToDes > -0.2) {
				lcd.clear();
				lcd.home();
				lcd.print("Password: ");

				onDestination = !onDestination;
			} else {
				lcd.clear();
				lcd.home();
				// lcd.print(distanceLAT);
				// lcd.setCursor(0, 1);
				// lcd.print(distanceLNG);
				lcd.print("Prox: ");
				lcd.setCursor(0, 1);
				lcd.print(disToDes);
			}
		}
	} else {
		// Keypad | LCD-scherm
		giveData();
		if(!passwordBeingReset) {
			if(dataCount == passwordLength - 1) {
				if(!strcmp(data, passWord[nextLocation])) {
					//Serial.println("Correct!");
					lcd.clear();
					lcd.home();
					lcd.print("Correct!");
					
					writeLED('B', nextLocation);
					FastLED.show();
					
					nextLocation++;
					onDestination = !onDestination;
				} else if(!strcmp(data, passWordReset)) {
					//Serial.println("New Pass: ");
					lcd.clear();
					lcd.home();
					lcd.print("New Pass: ");

					passwordBeingReset = !passwordBeingReset;
				} else {
					//Serial.println("Incorrect!");
					//Serial.println("Try Again...");
					lcd.clear();
					lcd.home();
					lcd.print("Incorrect!");
					lcd.setCursor(0, 1);
					lcd.print("Try again...");
				}
				clearData();
			}
		} else {
			for(int i = 0; i < passwordLength - 1; i++) {
				passWord[nextLocation][i] = data[i];
			}
			if(dataCount == passwordLength - 1) {
				//Serial.print("New Pass: ");
				//Serial.println(passWord);
				lcd.home();
				lcd.print("New Pass: ");
				lcd.print(passWord[nextLocation]);
				lcd.print("!");
				clearData();
				passwordBeingReset = !passwordBeingReset;
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
		//Serial.println(data);
		lcd.setCursor(0, 1);
		lcd.print(data);
	}
	return data;
}

void clearData() {
	for(int i = 0; i < passwordLength; i++) {
		data[i] = 0;
	}
	dataCount = 0;
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
}