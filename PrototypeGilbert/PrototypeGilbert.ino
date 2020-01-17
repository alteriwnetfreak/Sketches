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
//Connect to pins: 2 to 4, A3 to A0
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
byte colPins[COLS] = { 2, 3, 4 };
byte rowPins[ROWS] = { A3, A2, A1, A0 };
Keypad customKeypad = Keypad(makeKeymap(numPad), rowPins, colPins, ROWS, COLS);


// Global Variables | Keypad
#define passwordLength 6 // -1, wegens "closing char": (\n)
#define latlngAmount 12
#define latCOsize 10 // -1, wegens "closing char": (\n)
#define lngCOsize 9 // -1, wegens "closing char": (\n)

char data[passwordLength] = "";
char COdata[latCOsize] = "";
byte dataCount = 0;

char programmerMode[passwordLength] = "2#111";
byte COposition;


//*********************************************
// Include GPS | Software Serial
//*********************************************
#include <TinyGPS++.h>
// #include <SoftwareSerial.h>

TinyGPSPlus gps;
// SoftwareSerial gpSS(3, 4); // Pins voor de GPS: TX-pin 3, RX-pin 4
byte nextLocation = 0;
bool onDestination = false;
float LATDifference, LONGDifference, disToDes;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

long changeLCDtimer = 0;
bool waitForLCD = false;


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
byte passWordIncorrect = 0;
byte passwordScore = 0;
int score;


//*********************************************
// Include FastLED
//*********************************************
#include <FastLED.h>

#define PIN 6
#define NUM_LEDS 12
CRGB leds[NUM_LEDS];

bool colorChange = false;


//*********************************************
// Include Gyroscoop
//*********************************************
#include <Wire.h>

long accelX, accelY, accelZ;
float pitch, roll;
int tiltFactor;
byte tiltMax = 15;
float gyroGameCO[2][2] = {
	{ 52.024651, 5.555741 },
	{ 52.027860, 5.556520 }
};

bool leanTooFar = true;
bool gyrogameSetup = true;
bool notYetAtStartGyroGame = true;
bool gyrogameFinished = false;
bool gyrogameShowLCD = false;


// Variabelen voor ProgrammerMode
bool pmMode = false;
byte pmSwitch = 0;
byte pmState = 0;

// States and phases of the game
bool gameFinished = false;
bool phaseJustEnded;
byte gamePhase = 0;
byte endPhase = 4;
char phasePass[3][passwordLength] = { "20011", "30011", "40011" };

// Knop
long knopIngedrukt = 0;
byte knopStatus = 0;

//Timers
long rememberTime = 0;
bool rememberState = false;
long timeTillGyrogameEnds;
long gyroTimerOnLcd;
unsigned long timeBeforePause = 27000;
unsigned long nextPhaseBegin = 0;


//*********************************************
// SETUP
//*********************************************
void setup() 
{
	// Initialize Serial | SS | LCD
	Serial.begin(9600);
	// gpSS.begin(9600);
	lcd.begin(16, 2);
	lcd.clear();

	// Read EEPROM for coördinates en passwords
	EEPROM_read();

	// Initialize the leds
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	nextPhaseBegin = millis();

	// Initialize Gyro
	setupMPU();
}


//*********************************************
// LOOP
//*********************************************
void loop()
{
	//*********************************************
	// Code for button pressed, checking if button is pressed long enough
	//*********************************************
	byte knop = digitalRead(5);
	if(knop != knopStatus) { // knop changes constantly, so this codes runs constantly
		knopStatus = knop;
		if(knop == 1) { knopIngedrukt = millis(); } // knopIngedrukt gets the value of millis() | this is not updated while knop stays 1
		else { // each time knop is 0, the difference between millis() and knopIngedrukt is checked
			if(!gameFinished) {
				if(millis() - knopIngedrukt >= 15000) { // If the difference is more than n seconds
					rememberTime = millis();
					knopIngedrukt = 0;
					lcd.clear();
					lcd.home();
					if(pmSwitch == 0) { pmSwitch = 1; } // Go into pmMode
					else if(pmSwitch >= 1) { // Get out of pmMode
						waitForLCD = true;
						pmSwitch = 0;
						pmMode = false;
					}
					lcd.clear();
				}
			}
		}
	}
	
	//*********************************************
	// Code for LED's, checking which has to turn what color
	//*********************************************
	for(byte i = 0; i < NUM_LEDS; i++) {
		if(!gameFinished) { // LED logic when game is going
			if(gamePhase != 2) {
				if(passwordCorrect[i] == 1) { 	   writeLED(2, i); }
				else if(passwordCorrect[i] == 2) { writeLED(1, i); }
				else if(nextLocation == i) {  	   writeLED(3, i); }
				else {						  	   writeLED(0, i); }
			} else {
				if(abs(tiltFactor) < i) { writeLED(0, i); }
				else {
					if(abs(tiltFactor) > 5) { writeLED(4, i); }
					if(abs(tiltFactor) < 6) { writeLED(3, i); }
				}
			}
		} else { // LED logic when a phase is over (waiting to go to the next phase)
			if(gamePhase != 4) { // Make all LED's the same color and blink
				if(colorChange) { writeLED(0, i); }
				else {		  	  writeLED(3, i); }
				// Every N ms colorChange is updated, making the LED's blink
				if(millis() - rememberTime > 500) {
					rememberTime = millis();
					colorChange = !colorChange;
				}
			} else { // The game is over, so you get another animation
				byte x = millis() / 100 % NUM_LEDS; // Using millis() for the animation
				for(int i = 0; i < NUM_LEDS; i++) {
					if(i == x) { writeLED(0, i); }
					else { 		 writeLED(3, i); }
				}
			}
		}
		FastLED.show();
	}

	//*********************************************
	// Main code
	//*********************************************
	if(!waitForLCD) { 
		if(!gameFinished) {
			if(!pmMode) { // pmMode off 
				giveData();
				if(pmSwitch == 0) { // pmSwitch = 0, Not in ProgrammerMode | Should be the game
					if(millis() - nextPhaseBegin < timeBeforePause && nextLocation < latlngAmount / (2 - constrain(gamePhase, 0, 1))) {
						// if(onDestination) {
							// lcd.clear();
							lcd.home();
							lcd.print("Password: ");
							if(dataCount == passwordLength - 1) {
								if(!strcmp(data, passWord[nextLocation])) { // Password Correct | Go on to next point
									passWordIncorrect = 0;
									changeLCDtimer = millis();
									lcd.clear();
									waitForLCD = true;
									passwordCorrect[nextLocation] = 1;
									onDestination = false;
								} else { // Password Incorrect | try again
									passWordIncorrect++;
									if(passWordIncorrect == 3) {
										passwordCorrect[nextLocation] = 2;
										onDestination = false;
									}
									changeLCDtimer = millis();
									lcd.clear();
									waitForLCD = true;
								}
							}
						// } else { // Not at the desired location | show disToDes on LCD
							// while(Serial.available() > 0) {
							// 	gps.encode(Serial.read());
							// }
							// if(gps.location.isUpdated()) { // Every time the GPS get's a new location
							// 	// Code for Coordinates from GPS
							// 	LATDifference = gps.location.lat() - latlngCO[nextLocation][0];
							// 	LONGDifference = gps.location.lng() - latlngCO[nextLocation][1];
							// 	disToDes = sqrt(sq(LATDifference) + sq(LONGDifference)) * 65000;

							// 	Serial.print("Latitude: ");
							// 	Serial.print(gps.location.lat(), 6); 
							// 	Serial.print("\t");
							// 	Serial.print("Longitude: ");
							// 	Serial.print(gps.location.lng(), 6);
							// 	Serial.print("\t");
							// 	Serial.print("Latitude dif: ");
							// 	Serial.print(LATDifference, 6); 
							// 	Serial.print("\t");
							// 	Serial.print("Longitude dif: ");
							// 	Serial.print(LONGDifference, 6);
							// 	Serial.print("\t");
							// 	Serial.print("Distance: ");
							// 	Serial.println(disToDes);

							// 	lcd.clear();
							// 	lcd.home();
							// 	lcd.print("Distance to Des:");
							// 	lcd.setCursor(0, 1);
							// 	lcd.print(disToDes);

							// 	if(disToDes < 5) {
							// 		onDestination = true;
							// 		lcd.clear();
							// 	}
							// 	clearData();
							// }
						// }
					} else if(gamePhase == 2) { // Gyro Game
						if(!gyrogameFinished) { // While the game is still going
							if(gyrogameSetup) {
								timeTillGyrogameEnds = millis() + 300000;
								for(byte i = 0; i < latlngAmount; i++) {
									if(passwordCorrect[i] == 1) {
										passwordScore++;
									}
								}
								gyrogameSetup = false;
							}
							recordAccelRegisters();
							printData();

							while(Serial.available() > 0) {
								gps.encode(Serial.read());
							}
							if(gps.location.isUpdated()) { // Every time the GPS get's a new location
								// Code for Coordinates from GPS
								LATDifference = gps.location.lat() - gyroGameCO[notYetAtStartGyroGame][0];
								LONGDifference = gps.location.lng() - gyroGameCO[notYetAtStartGyroGame][1];
								disToDes = sqrt(sq(LATDifference) + sq(LONGDifference)) * 65000;

								lcd.setCursor(0, 1);
								lcd.print("Distance: ");
								lcd.print(disToDes);

								if(disToDes < 5) {
									if(notYetAtStartGyroGame) {
										notYetAtStartGyroGame = false;
									} else {
										gyrogameFinished = true;
										score = gyroTimerOnLcd;
									}
									lcd.clear();
								}
								clearData();
							}
						} else { // Gyroscope game is done, show text for a few seconds
							if(!gyrogameShowLCD) {
								rememberTime = millis();
								gyrogameShowLCD = true;
							}
							lcd.home();
							if(gyroTimerOnLcd == 0) {
								lcd.print("You're too late");
								lcd.setCursor(0, 1);
								lcd.print("...");
							} else if(passwordScore == 0) {
								lcd.print("It's gone wrong");
								lcd.setCursor(0, 1);
								lcd.print("...");
							} else {
								lcd.print("You did it! You");
								lcd.setCursor(0, 1);
								lcd.print("got to the end!");
							}

							// After N seconds, change the Phase, so the game ends
							if(rememberTime + 3000 < millis()) { gamePhase = 3; }
						}
					} else { // You have been to all the locations/points || time is up
						if(gamePhase != endPhase) {
							gameFinished = true;
							phaseJustEnded = true;
						}
						clearData();
					}
				} else if(pmSwitch == 1) { // pmSwitch = 1, First stage of pmMode | logging in
					lcd.home();
					lcd.print("PMMode pass: ");
					if(dataCount == passwordLength - 1) {
						if(!strcmp(data, programmerMode)) { // If pmMode pass is correct | Go on to next phase
							Serial.println("Old Coördinates: ");
							EEPROM_read();
							pmSwitch = 2;
							lcd.clear();
						} else { // If pmMode pass is incorrect | Go back to the game
							changeLCDtimer = millis();
							lcd.clear();
							waitForLCD = true;
						}
						clearData();
					}
				} else { // pmSwitch = 2, Second stage of pmMode | Changing values
					lcd.home();
					lcd.print("Which location?");
					if(dataCount == 2) { // Als het aantal karakters dat nodig is is bereikt
						COposition = atoi(data);
						if(COposition == 0 || COposition > latlngAmount) { // If typed is not compatible with the list, try again
						    changeLCDtimer = millis();
							lcd.clear();
						    waitForLCD = true;
						} else { // Go on to changing the value
							changeLCDtimer = millis();
							lcd.clear();
							waitForLCD = true;
							pmMode = true;
						}
						clearData();
					}
				}
			} else { // pmMode on
				giveCoordinate();

				lcd.home();
				switch(pmState) {
					case 0: // Change LAT
						lcd.print("LAT: ");
						if(dataCount == latCOsize - 1) { // If the amount of characters that's needed has been reached
							sizeCO = (COposition - 1) * (2*sizeof(float)); // Location in the EEPROM
							EEPROM.put(sizeCO, atof(COdata));
							EEPROM_read(); // Variables are updated
							changeLCDtimer = millis();
							lcd.clear();
							waitForLCD = true;
							clearData();
							pmState++;
						}
						break;
					case 1: // Change LONG
						lcd.print("LONG: ");
						if(dataCount == lngCOsize - 1) { // If the amount of characters that's needed has been reached
							sizeCO = (COposition - 1) * (2*sizeof(float)) + 4; // Location in the EEPROM
							EEPROM.put(sizeCO, atof(COdata));
							EEPROM_read(); // Variables are updated
							changeLCDtimer = millis();
							lcd.clear();
							waitForLCD = true;
							clearData();
							pmState++;
						}
						break;
					case 2: // Change PASS
						lcd.print("Pass: ");
						if(dataCount == passwordLength - 1) { // If the amount of characters that's needed has been reached 
							sizePass = (COposition - 1) * (12*sizeof(char)) + sizeof(latlngCO); // Location in the EEPROM
							EEPROM.put(sizePass, COdata);
							EEPROM_read(); // Variables are updated
							pmMode = false;
							pmSwitch = 2;
							pmState = 0;
							clearData();
							lcd.clear();
						}
						break;
				}
			}
		} else {
			if(gamePhase == 4) {
				changeLCDtimer = millis();
				lcd.clear();
				waitForLCD = true;
				pmMode = false;
				pmSwitch = 0;
				pmState = 0;
				phaseJustEnded = !phaseJustEnded;
			} else {
				if(phaseJustEnded) {
					changeLCDtimer = millis();
					lcd.clear();
					waitForLCD = true;
					pmMode = false;
					pmSwitch = 0;
					pmState = 0;
					phaseJustEnded = !phaseJustEnded;
				}
				if(gamePhase < endPhase) {
					giveData();
					if(dataCount == passwordLength - 1) {
						lcd.clear();
						lcd.home();
						if(!strcmp(data, phasePass[constrain(gamePhase, 0, 2)])) { // Password Correct | Go on to next phase
							nextPhaseBegin = millis();
							gameFinished = false;
							gamePhase++;
							nextLocation = latlngAmount/2 * gamePhase;
							for(int i = 0; i < nextLocation; i++) {
								if(passwordCorrect[i] == 0) { passwordCorrect[i] = 2; }
							}
						}
						clearData();
					}
				}
			}
		}
	} else { // This is the part where most of the text on the LCD is handled
		lcd.home();
		if(!rememberState) { // This code mostly follows the same structure as the code above, to indicate what text needs to be displayed
			if(!gameFinished) {
				if(!pmMode) {
					if(pmSwitch == 0) {
						if(millis() - nextPhaseBegin < timeBeforePause && nextLocation < latlngAmount / (2 - constrain(gamePhase, 0, 1))) {
							if(dataCount == passwordLength - 1) {
								lcd.clear();
								lcd.home();
								if(!strcmp(data, passWord[nextLocation])) { // Password correct
									lcd.print("Correct! Go to");
									lcd.setCursor(0, 1);
									lcd.print(" next location");
									nextLocation++;
								} else { // Password incorrect
									if(passWordIncorrect == 3) {
										lcd.print("Pass failed. Go");
										lcd.setCursor(0, 1);
										lcd.print(" to next point!");
										nextLocation++;
										passWordIncorrect = 0;
									} else {
										lcd.print("Incorrect! Pls");
										lcd.setCursor(0, 1);
										lcd.print(" try again...");
									}
								}
								clearData();
							}
						}
					} else if(pmSwitch == 1) {
						lcd.print("Onjuist!");
						pmSwitch = 0;
					} else {
						// When the given location is out of bounds
						if(COposition == 0 || COposition > latlngAmount) { lcd.print("Niet Geldig!"); }
					}
				} else {
					switch(pmState) { // Showing old information, for reference
						case 0:
							lcd.print("Old LAT: ");
							lcd.setCursor(0, 1);
							lcd.print(latlngCO[COposition - 1][0], 6);
							break;
						case 1:
							lcd.print("Old LONG: ");
							lcd.setCursor(0, 1);
							lcd.print(latlngCO[COposition - 1][1], 6);
							break;
						case 2:
							lcd.print("Old Pass: ");
							lcd.setCursor(0, 1);
							lcd.print(passWord[COposition - 1]);
							break;
					}
				}
			} else {
				switch(gamePhase) { // Text for every time gamePhase changes
					case 0:
						lcd.print("End of Phase 1!");
						lcd.setCursor(0, 1);
						lcd.print("Return to Circle");
						break;
					case 1:
						lcd.print("End of Phase 2!");
						lcd.setCursor(0, 1);
						lcd.print("Return to Circle");
						break;
					case 2:
						lcd.print("End of Phase 3!");
						lcd.setCursor(0, 1);
						lcd.print("Return to Circle");
						break;
					case 3:
						lcd.print("End of Game. Go");
						lcd.setCursor(0, 1);
						lcd.print(" to The Circle.");
						break;
					case 4: // endPhase | here is shown the score that is earned
						lcd.print("Score: ");
						lcd.print(score);
						lcd.print("s");
						lcd.setCursor(0, 1);
						lcd.print("Bonus: ");
						lcd.print(passwordScore * 10);
						lcd.print("s");
						break;
				}
			}
			rememberState = true;
		}
		if(millis() - changeLCDtimer > 2000) {
			lcd.clear();
			lcd.home();
			waitForLCD = false;
			rememberState = false;
		}
	}
}


//*********************************************
// FUNCTIONS
//*********************************************

// Functions Keypad
char* giveData() {
	char customKey = customKeypad.getKey(); // char from keypad is saved in customKey
	if(customKey) { // customKey is updated
		data[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(data);
		// Serial.println(data);
	}
	return data;
}
char* giveCoordinate() {
	char customKey = customKeypad.getKey(); // char from keypad is saved in customKey
	if(customKey) { // customKey is updated
		COdata[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(COdata);
		// Serial.println(COdata);
	}
	return COdata;
}
void clearData() { // data, COdata and dataCount are emptied for next use
	for(byte i = 0; i < passwordLength; i++) { data[i] = 0; }
	for(byte i = 0; i < latCOsize; i++) { 	   COdata[i] = 0; }
	dataCount = 0;
}

// EEPROM Functions
void EEPROM_read() { // Transfering data from EEPROM to variables for easy use || Happens when the Arduino starts up and when something changes
	for(byte i = 0; i < latlngAmount; i++) {
		// Address of EEPROM per instance of information
		sizeCO = i * (2*sizeof(float));							// Address on EEPROM for Coördinates
		sizePass = i * (12*sizeof(char)) + sizeof(latlngCO);	// Address on EEPROM for passwords
		// Getting the information per address out of EEPROM to a variable
		latlngCO[i][0] = EEPROM.get(sizeCO, valueF);		// Latitude is put in a variable
		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);	// Longitude is put in a variable
		memcpy(passWord[i], EEPROM.get(sizePass, valueC), sizeof(passWord[0])); // Password is put in a variable
		// Print the results
		Serial.print(latlngCO[i][0], 6);
		Serial.print("\t");
		Serial.print(latlngCO[i][1], 6);
		Serial.print("\t");
		Serial.println(passWord[i]);
	}
	Serial.println("");
}

// FastLED Functions
void writeLED(byte color, byte led) { // Desides what LED needs which color
	switch(color) {
		case 0: leds[led].setRGB(0, 0, 0); break;
		case 1: leds[led].setRGB(20, 0, 0); break;
		case 2: leds[led].setRGB(0, 20, 0); break;
		case 3: leds[led].setRGB(0, 0, 20); break;
		case 4: leds[led].setRGB(20, 10, 0); break;
	}
}

// Gyro functions
void setupMPU(){
	Wire.begin();
	Wire.beginTransmission(0x68);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);

	Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
	Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
	Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
	Wire.endTransmission();  
	Wire.beginTransmission(0b1101000); //I2C address of the MPU
	Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
	Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
	Wire.endTransmission(); 
	Wire.beginTransmission(0b1101000); //I2C address of the MPU
	Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
	Wire.write(0b00000000); //Setting the accel to +/- 2g
	Wire.endTransmission(); 
}
void recordAccelRegisters() { // Getting numbers from the MPU 6050
	Wire.beginTransmission(0b1101000); //I2C address of the MPU
	Wire.write(0x3B); //Starting register for Accel Readings
	Wire.endTransmission();
	Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
	while(Wire.available() < 6);
	accelX = Wire.read() <<8 | Wire.read(); //Store first two bytes into accelX
	accelY = Wire.read() <<8 | Wire.read(); //Store middle two bytes into accelY
	accelZ = Wire.read() <<8 | Wire.read(); //Store last two bytes into accelZ
	processAccelData();
}
void processAccelData() { // processing the numbers, so we can actually read and use them
	pitch = (atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI);
	roll = (atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI);
	tiltFactor = roll * 2; // Extra var to use in the gyro game
}
void printData() { // Function for debugging purposes, showing us the pitch, roll and tiltFactor
	if(rememberState) {
		rememberTime = millis();
		if(leanTooFar) {
			if(passwordScore < 20 && passwordScore > 0) {
				passwordScore--;
				if(passwordScore == 0) {
					gyrogameFinished = true;
					score = gyroTimerOnLcd;
				}
			}
			rememberState = false;
		}
	} else {
		leanTooFar = false;
		lcd.setCursor(0, 1);
		lcd.print("Level case: ");
		lcd.print((millis() - rememberTime) / 1000);
		if(millis() - rememberTime > 5000) {
			rememberState = true;
			lcd.clear();
		}
	}
	// Checking if gyro is tilted too far or not
	if(abs(tiltFactor) > 7) { leanTooFar = true; }

	lcd.home();
	lcd.print("Time: ");
	gyroTimerOnLcd = (timeTillGyrogameEnds - millis()) / 1000;
	if(gyroTimerOnLcd <= 0) { // If timer gets to 0 (zero) | finish the gyro game
		gyroTimerOnLcd = 0;
		gyrogameFinished = true;
	}
	lcd.print(gyroTimerOnLcd);
	lcd.setCursor(11, 0);
	lcd.print("S:");
	lcd.print(passwordScore);
}

/*
	Voor password:
	//__ 3 pogingen, na 3 pogingen 	-->		password fout, LED wordt rood
									-->		password goed, LED wordt blauw
		 ga naar volgende punt

	//__ Meer LEDs toevoegen

	//__ Wachtwoord in plaats van knop voor phase
		 bij punt 6 en 12 code invoeren om verder te kunnen gaan met het spel

	//__ knop PMMode langer (15s)

	//__ bij te ver hellen 			--> 	"Danger!", 1x een punt eraf
	//__ na 4-5 seconden 			--> 	door gaan met het spel/rekenen

	Nog niet getest -->
	//__ met phase 3 (gyro game) naar punt lopen
		 bij punt aangekomen 			--> 	moet timer aflopen, gyro activeren, naar ander punt (The Circle) lopen
		 bij eindpunt "The Circle" 		--> 	timer stoppen
		 Final score laten zien
*/