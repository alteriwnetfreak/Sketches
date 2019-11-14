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
// // "ANALOG":
byte colPins[COLS] = { 2, 5, 7 };
byte rowPins[ROWS] = { A3, A2, A1, A0 };

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

// Variables | Keypad
int dataCount = 0;
#define passwordLength 6

char data[passwordLength] = "";
char passWord[][passwordLength] = {
	"21199", "69666", "21420", "11111", "11*22", "22#33", "03159", "00004",  "**#*#", "##*#*"
};
char passWordReset[passwordLength] = "2#111";
bool passwordBeingReset = false;


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

float locatie[][2] = { 
	{ 52.025210, 5.555854 },
	{ 52.026478, 5.556792 },
	{ 52.025814, 5.557516 },
	{ 52.024656, 5.556728 },
	{ 52.023989, 5.556685 },
	{ 52.024649, 5.555698 },
	{ 01.123456, 0.123456 },
	{ 12.345678, 2.345678 },
	{ 23.456789, 3.456789 },
	{ 34.567890, 4.567890 }
};
int nextLocation = 0;

float myLAT, myLNG;
float distanceLAT, distanceLNG;
float disToDes;
int multiplier = 1000000;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


//*********************************************
// Include FastLED
//*********************************************
// #include <FastLED.h>

// #define PIN 6
// #define NUM_LEDS 1

// CRGB leds[NUM_LEDS];


//*********************************************
// Include Gyroscoop
//*********************************************
#include <Wire.h>

long accelX, accelY, accelZ;
int pitch, roll;



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
	// for(int x=0; x<NUM_LEDS; x++){
	//     writeLED('R', x);
	// }
	// FastLED.show();

	// Print start of the game
	lcd.print("WELCOME!");
	lcd.setCursor(0, 1);
	lcd.print("Pls go outside");
	
	// Initialize interface to the MPU6050
	Wire.begin();
	// setupMPU();
}

//*********************************************
// Loop
//*********************************************
void loop() {
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
				lcd.print("Proximity:");
				lcd.setCursor(0, 1);
				lcd.print(disToDes * 100);
			}
		}
	} else {
		giveData();
		if(!passwordBeingReset && dataCount == passwordLength - 1) {
			if(!strcmp(data, passWord[nextLocation])) {
				//Serial.println("Correct!");
				lcd.clear();
				lcd.home();
				lcd.print("Correct!");
				
				// writeLED('B', nextLocation);
				// FastLED.show();
				
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
		} else {
			givePassword();
		}
		
		// // Gyroscope
		// recordAccelRegisters();
		// printData();
		// delay(100);
	}
}

// // Functions Gyroscoop
// void setupMPU(){
// 	Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
// 	Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
// 	Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
// 	Wire.endTransmission();  
// 	Wire.beginTransmission(0b1101000); //I2C address of the MPU
// 	Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
// 	Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
// 	Wire.endTransmission(); 
// 	Wire.beginTransmission(0b1101000); //I2C address of the MPU
// 	Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
// 	Wire.write(0b00000000); //Setting the accel to +/- 2g
// 	Wire.endTransmission(); 
// }
// void recordAccelRegisters() {
// 	Wire.beginTransmission(0b1101000); //I2C address of the MPU
// 	Wire.write(0x3B); //Starting register for Accel Readings
// 	Wire.endTransmission();
// 	Wire.requestFrom(0b1101000, 6); //Request Accel Registers (3B - 40)
// 	while(Wire.available() < 6);
// 	accelX = Wire.read() <<8 | Wire.read(); //Store first two bytes into accelX
// 	accelY = Wire.read() <<8 | Wire.read(); //Store middle two bytes into accelY
// 	accelZ = Wire.read() <<8 | Wire.read(); //Store last two bytes into accelZ
// 	processAccelData();
// }
// void processAccelData(){
// 	// Calculating the pitch (rotation around Y-axis) and roll (rotation around X-axis)
// 	pitch = atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI;
// 	roll = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI;
// }

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
char givePassword() {
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
	return passWord;
}
void clearData() {
	for(int i = 0; i < passwordLength; i++) {
		data[i] = 0;
	}
	dataCount = 0;
}

// functions LEDs
// void writeLED(char color, int led) {
// 	if (color == 'R'){ 
//     	leds[led] = CRGB::Red; 
// 	} else if (color == 'G') { 
//     	leds[led] = CRGB::Green; 
// 	} else if (color == 'B') { 
//     	leds[led] = CRGB::Blue; 
// 	} else if (color == ' ') { 
//     	leds[led] = CRGB::Black; 
// 	}
// }