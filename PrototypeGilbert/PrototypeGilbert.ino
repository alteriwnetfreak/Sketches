//----------------------------------------------------
// PrototypeGilbert
//----------------------------------------------------

//*********************************************
// Including keypad
//*********************************************
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 3;
char numPad[ROWS][COLS] = 
{
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
#define passwordLength 6 // -1, cuz of "closing char": (\n)
#define locationAmount 12
#define LATsize 10 // -1, cuz of "closing char": (\n)
#define LONGsize 9 // -1, cuz of "closing char": (\n)

char data[passwordLength] = "";
char locationData[LATsize] = "";
byte dataCount = 0;

char programmerMode[passwordLength] = "2#111";
byte nrOfLocation;


//*********************************************
// Include GPS
//*********************************************
#include <TinyGPS++.h>

TinyGPSPlus gps;
byte nextLocation = 0;
bool onDestination = false;
float LATDifference, LONGDifference, disToDes, direction;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

long changeLCDtimer = 0;
bool waitForLCD = false;


//*********************************************
// Include EEPROM
//*********************************************
#include <EEPROM.h>

int sizeLocation, sizePassword;
float valueLocation;
char valuePassword[6];
float latlngLocation[locationAmount][2];
char passWord[locationAmount][passwordLength];
byte passwordCorrect[locationAmount] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
byte passWordIncorrect = 0;
byte passwordScore = 0;
int timeScore;


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
byte tiltThreshold = 12;
byte tiltMax = 15;
float gyroGameCO[2][2] = 
{
	{ 52.024425, 5.555132 },
	{ 52.027860, 5.556520 }
};

bool leanTooFar = true;
bool notYetAtStartGyroGame = true;
bool gyrogameSetup = true;
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
char phasePass[3][passwordLength] = { "10011", "20011", "30011" };

// button
long buttonPressed = 0;
byte buttonStatus = 0;

//Timers
long rememberTime = 0;
bool rememberState = false;
long timeTillGyrogameEnds;
long gyroTimerOnLcd;
unsigned long timeBeforePause = 2700000;
unsigned long nextPhaseBegin = 0;


//*********************************************
// SETUP
//*********************************************
void setup() 
{
	// Initialize Serial | LCD
	Serial.begin(9600);
	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("Waiting for game");
	lcd.setCursor(0, 1);
	lcd.print(" to start...!");

	// Read EEPROM for coördinates and passwords
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
	byte button = digitalRead(5);
	if(button != buttonStatus) // button changes constantly, so this codes runs constantly
	{ 
		buttonStatus = button;
		if(button == 1) { buttonPressed = millis(); } // buttonPressed gets the value of millis() | this is not updated while button stays 1
		else // each time button is 0, the difference between millis() and buttonPressed is checked
		{ 
			if(!gameFinished) 
			{
				if(millis() - buttonPressed >= 15000) // If the difference is more than n seconds
				{ 
					rememberTime = millis();
					buttonPressed = 0;
					lcd.clear();
					lcd.home();
					if(pmSwitch == 0) { pmSwitch = 1; } // Go into pmMode
					else if(pmSwitch >= 1) // Get out of pmMode
					{
						StartLCD();
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
	for(byte i = 0; i < NUM_LEDS; i++) 
	{
		if(!gameFinished) // LED logic when game is going
		{
			if(gamePhase != 2) 
			{
				if(passwordCorrect[i] == 1) { 	   writeLED(2, i); }
				else if(passwordCorrect[i] == 2) { writeLED(1, i); }
				else if(nextLocation == i) {  	   writeLED(3, i); }
				else {						  	   writeLED(0, i); }
			}
			else
			{
				if(abs(tiltFactor) < i) { writeLED(0, i); }
				else
				{
					if(abs(tiltFactor) > tiltThreshold - 3) { writeLED(4, i); }
					if(abs(tiltFactor) < tiltThreshold - 2) { writeLED(3, i); }
				}
			}
		}
		else // LED logic when a phase is over (waiting to go to the next phase)
		{
			if(gamePhase != 4) // Make all LED's the same color and blink
			{
				if(colorChange) { writeLED(0, i); }
				else {		  	  writeLED(3, i); }
				// Every N ms colorChange is updated, making the LED's blink
				if(millis() - rememberTime > 500) 
				{
					rememberTime = millis();
					colorChange = !colorChange;
				}
			}
			else // The game is over, so you get another animation
			{
				byte x = millis() / 100 % NUM_LEDS; // Using millis() for the animation
				for(int i = 0; i < NUM_LEDS; i++) 
				{
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
	GpsEncoding();
	if(!waitForLCD)
	{
		if(!gameFinished)
		{
			if(!pmMode) // pmMode off 
			{
				giveData();
				if(pmSwitch == 0) // pmSwitch = 0, Not in ProgrammerMode | Should be the game
				{
					if(millis() - nextPhaseBegin < timeBeforePause && nextLocation < locationAmount / (2 - constrain(gamePhase, 0, 1)))
					{
						if(onDestination)
						{
							lcd.clear();
							lcd.home();
							lcd.print("Password: ");
							if(dataCount == passwordLength - 1) 
							{
								if(!strcmp(data, passWord[nextLocation])) // Password Correct | Go on to next point
								{
									passWordIncorrect = 0;
									StartLCD();
									passwordCorrect[nextLocation] = 1;
									onDestination = false;
								}
								else // Password Incorrect | try again
								{
									passWordIncorrect++;
									if(passWordIncorrect == 3)
									{
										passwordCorrect[nextLocation] = 2;
										onDestination = false;
									}
									StartLCD();
								}
							}
						}
						else // Not at the desired location | show disToDes on LCD
						{
							ReadGPS(latlngLocation, nextLocation);
						}
					}
					else if(gamePhase == 2) // Gyro Game
					{
						if(!gyrogameFinished) // While the game is still going
						{
							if(gyrogameSetup)
							{
								rememberState = true;
								gyrogameSetup = false;
							}
							ReadGPS(gyroGameCO, notYetAtStartGyroGame);
							if(!notYetAtStartGyroGame)
							{
								recordAccelRegisters();
								printData();
							}
						}
						else // Gyroscope game is done, show text for a few seconds
						{
							if(!gyrogameShowLCD)
							{
								rememberTime = millis();
								gyrogameShowLCD = true;
							}
							lcd.home();
							if(gyroTimerOnLcd == 0)
							{
								lcd.print("You're too late");
								lcd.setCursor(0, 1);
								lcd.print("...");
							}
							else if(passwordScore == 0)
							{
								lcd.print("Something has");
								lcd.setCursor(0, 1);
								lcd.print(" gone wrong!");
							}
							else
							{
								lcd.print("You did it! You");
								lcd.setCursor(0, 1);
								lcd.print("got to the end!");
							}
							// After N seconds, change the Phase, so the game ends
							if(rememberTime + 3000 < millis()) { gamePhase = 3; }
						}
					} else // You have been to all the locations/points || time is up
					{
						if(gamePhase != endPhase)
						{
							gameFinished = true;
							phaseJustEnded = true;
						}
						clearData();
					}
				}
				else if(pmSwitch == 1) // pmSwitch = 1, First stage of pmMode | logging in
				{
					lcd.home();
					lcd.print("PMMode pass: ");
					if(dataCount == passwordLength - 1)
					{
						if(!strcmp(data, programmerMode)) // If pmMode pass is correct | Go on to next phase
						{
							Serial.println("Old Coördinates: ");
							EEPROM_read();
							pmSwitch = 2;
							lcd.clear();
						}
						else // If pmMode pass is incorrect | Go back to the game
						{
							StartLCD();
						}
						clearData();
					}
				} else // pmSwitch = 2, Second stage of pmMode | Changing values
				{
					lcd.home();
					lcd.print("Which location?");
					if(dataCount == 2) // Als het aantal karakters dat nodig is is bereikt
					{
						nrOfLocation = atoi(data);
						if(nrOfLocation == 0 || nrOfLocation > locationAmount) // If typed is not compatible with the list, try again
						{
						    StartLCD();
						}
						else // Go on to changing the value
						{
							StartLCD();
							pmMode = true;
						}
						clearData();
					}
				}
			}
			else // pmMode on
			{ 
				giveCoordinate();
				lcd.home();
				switch(pmState) 
				{
					case 0: // Change LAT
						lcd.print("LAT: ");
						if(dataCount == LATsize - 1) // If the amount of characters that's needed has been reached
						{
							sizeLocation = (nrOfLocation - 1) * (2*sizeof(float)); // Location in the EEPROM
							EEPROM.put(sizeLocation, atof(locationData));
							EEPROM_read(); // Variables are updated
							StartLCD();
							clearData();
							pmState++;
						}
						break;
					case 1: // Change LONG
						lcd.print("LONG: ");
						if(dataCount == LONGsize - 1) // If the amount of characters that's needed has been reached
						{
							sizeLocation = (nrOfLocation - 1) * (2*sizeof(float)) + 4; // Location in the EEPROM
							EEPROM.put(sizeLocation, atof(locationData));
							EEPROM_read(); // Variables are updated
							StartLCD();
							clearData();
							pmState++;
						}
						break;
					case 2: // Change PASS
						lcd.print("Pass: ");
						if(dataCount == passwordLength - 1) // If the amount of characters that's needed has been reached 
						{
							sizePassword = (nrOfLocation - 1) * (12*sizeof(char)) + sizeof(latlngLocation); // Location in the EEPROM
							EEPROM.put(sizePassword, locationData);
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
		}
		else // Game is finished / paused
		{
			if(gamePhase == 4)
			{
				StartLCD();
				pmMode = false;
				pmSwitch = 0;
				pmState = 0;
				phaseJustEnded = !phaseJustEnded;
			}
			else
			{
				if(phaseJustEnded)
				{
					StartLCD();
					pmMode = false;
					pmSwitch = 0;
					pmState = 0;
					phaseJustEnded = !phaseJustEnded;
				}
				if(gamePhase < endPhase)
				{
					giveData();
					if(dataCount == passwordLength - 1)
					{
						lcd.clear();
						lcd.home();
						if(!strcmp(data, phasePass[constrain(gamePhase, 0, 2)])) // Password Correct | Go on to next phase
						{ 
							nextPhaseBegin = millis();
							gameFinished = false;
							gamePhase++;
							nextLocation = locationAmount/2 * gamePhase;
							for(int i = 0; i < nextLocation; i++)
							{
								if(passwordCorrect[i] == 0) { passwordCorrect[i] = 2; }
							}
						}
						clearData();
					}
				}
			}
		}
	}
	else // This is the part where most of the text on the LCD is handled
	{
		lcd.home();
		if(!rememberState) // This code mostly follows the same structure as the code above, to indicate what text needs to be displayed
		{
			if(!gameFinished)
			{
				if(!pmMode)
				{
					if(pmSwitch == 0)
					{
						if(millis() - nextPhaseBegin < timeBeforePause && nextLocation < locationAmount / (2 - constrain(gamePhase, 0, 1)))
						{
							if(dataCount == passwordLength - 1) 
							{
								lcd.clear();
								lcd.home();
								if(!strcmp(data, passWord[nextLocation])) // Password correct
								{
									lcd.print("Correct! Go to");
									lcd.setCursor(0, 1);
									lcd.print(" next location");
									nextLocation++;
								}
								else // Password incorrect
								{
									if(passWordIncorrect == 3)
									{
										lcd.print("Pass failed. Go");
										lcd.setCursor(0, 1);
										lcd.print(" to next point!");
										nextLocation++;
										passWordIncorrect = 0;
									}
									else
									{
										lcd.print("Incorrect! Pls");
										lcd.setCursor(0, 1);
										lcd.print(" try again...");
									}
								}
								clearData();
							}
						}
					}
					else if(pmSwitch == 1)
					{
						lcd.print("Onjuist!");
						pmSwitch = 0;
					}
					else
					{
						// When the given location is out of bounds
						if(nrOfLocation == 0 || nrOfLocation > locationAmount) { lcd.print("Invalid!"); }
					}
				}
				else
				{
					switch(pmState) // Showing old information, for reference
					{
						case 0:
							lcd.print("Old Latitude: ");
							lcd.setCursor(0, 1);
							lcd.print(latlngLocation[nrOfLocation - 1][0], 6);
							break;
						case 1:
							lcd.print("Old Longitude: ");
							lcd.setCursor(0, 1);
							lcd.print(latlngLocation[nrOfLocation - 1][1], 6);
							break;
						case 2:
							lcd.print("Old Password: ");
							lcd.setCursor(0, 1);
							lcd.print(passWord[nrOfLocation - 1]);
							break;
					}
				}
			}
			else
			{
				switch(gamePhase) // Text for every time gamePhase changes
				{
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
					case 4: // endPhase | here is shown the Score that is earned
						lcd.print("Score: ");
						lcd.print(timeScore);
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
		if(millis() - changeLCDtimer > 2000)
		{
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
// Keypad
char* giveData() 
{
	char customKey = customKeypad.getKey(); // char from keypad is saved in customKey
	if(customKey) // customKey is updated
	{ 
		data[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(data);
		// Serial.println(data);
	}
	return data;
}
char* giveCoordinate() 
{
	char customKey = customKeypad.getKey(); // char from keypad is saved in customKey
	if(customKey) // customKey is updated
	{ 
		locationData[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(locationData);
		// Serial.println(locationData);
	}
	return locationData;
}
void clearData() // data, locationData and dataCount are emptied for next use
{ 
	for(byte i = 0; i < passwordLength; i++) { data[i] = 0; }
	for(byte i = 0; i < LATsize; i++) { 	   locationData[i] = 0; }
	dataCount = 0;
}

// LCD
void StartLCD()
{
	changeLCDtimer = millis();
	lcd.clear();
	waitForLCD = true;
}

// EEPROM
void EEPROM_read() // Transfering data from EEPROM to variables for easy use || Happens when the Arduino starts up and when something changes
{ 
	for(byte i = 0; i < locationAmount; i++) 
	{
		// Address of EEPROM per instance of information
		sizeLocation = i * (2*sizeof(float));							// Address on EEPROM for Coördinates
		sizePassword = i * (12*sizeof(char)) + sizeof(latlngLocation);	// Address on EEPROM for passwords
		// Getting the information per address out of EEPROM to a variable
		latlngLocation[i][0] = EEPROM.get(sizeLocation, valueLocation);		// Latitude is put in a variable
		latlngLocation[i][1] = EEPROM.get(sizeLocation + 4, valueLocation);	// Longitude is put in a variable
		memcpy(passWord[i], EEPROM.get(sizePassword, valuePassword), sizeof(passWord[0])); // Password is put in a variable
		// Print the results
		Serial.print(latlngLocation[i][0], 6);
		Serial.print("\t");
		Serial.print(latlngLocation[i][1], 6);
		Serial.print("\t");
		Serial.println(passWord[i]);
	}
	Serial.println("");
}

// FastLED
void writeLED(byte color, byte led) // Desides what LED needs which color
{ 
	switch(color) 
	{
		case 0: leds[led].setRGB(0, 0, 0); break;
		case 1: leds[led].setRGB(20, 0, 0); break;
		case 2: leds[led].setRGB(0, 20, 0); break;
		case 3: leds[led].setRGB(0, 0, 20); break;
		case 4: leds[led].setRGB(20, 10, 0); break;
	}
}

// Gyro
void setupMPU()
{
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
void recordAccelRegisters() // Getting numbers from the MPU 6050
{ 
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
void processAccelData() // processing the numbers, so we can actually read and use them
{ 
	pitch = (atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI);
	roll = (atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI);
	tiltFactor = roll * 2; // Extra var to use in the gyro game
}
void printData() // Shows all the necessary data for the gyro game
{
	if(rememberState)
	{
		rememberTime = millis();
		if(leanTooFar)
		{
			lcd.clear();
			if(passwordScore < 20 && passwordScore > 0)
			{
				passwordScore--;
				if(passwordScore == 0 || passwordScore > 200)
				{
					gyrogameFinished = true;
					timeScore = gyroTimerOnLcd;
				}
			}
			rememberState = false;
			leanTooFar = false;
		}
	}
	else
	{
		lcd.setCursor(0, 1);
		lcd.print("Level case: ");
		lcd.print((millis() - rememberTime) / 1000);
		if(millis() - rememberTime > 5000) 
		{
			rememberState = true;
			lcd.clear();
		}
	}
	// Checking if gyro is tilted too far or not
	if(abs(tiltFactor) > tiltThreshold) { leanTooFar = true; }
	lcd.home();
	lcd.print("Time: ");
	gyroTimerOnLcd = (timeTillGyrogameEnds - millis()) / 1000;
	if(gyroTimerOnLcd <= 0) // If timer gets to 0 (zero) | finish the gyro game
	{ 
		gyroTimerOnLcd = 0;
		gyrogameFinished = true;
	}
	lcd.print(gyroTimerOnLcd);
	lcd.setCursor(11, 0);
	lcd.print("S:");
	lcd.print(passwordScore);
}

// GPS
void GpsEncoding() // Keeps looking for a fix while the game is playing, despite where you are 
{
	while(Serial.available() > 0)
	{
		char c = Serial.read();

		gps.encode(c);
		// Serial.write(c); // used for debugging
	}
}
void ShowDirection(byte placeOnLCDh, byte placeOnLCDv) // This function is used to show in which direction you have to go
{
	lcd.setCursor(placeOnLCDh, placeOnLCDv);
	if(direction > 0.414 || direction < -0.414) // Checks if you have to go North or South
	{
		if(LATDifference < 0) { lcd.print("N"); }
		if(LATDifference > 0) { lcd.print("S"); }
	}
	lcd.setCursor(9, 1);
	lcd.print("/");
	if(direction < 2.414 && direction > -2.414) // Checks if you have to go East or West
	{
		if(LONGDifference < 0) { lcd.print("E"); }
		if(LONGDifference > 0) { lcd.print("W"); }
	}
}
void ReadGPS(float position[][2], byte number) // This is where most calculations are done, concerning the GPS
{
	if(gps.location.isUpdated()) // Every time the GPS get's a new location
	{
		// Code for Coordinates from GPS
		LATDifference = gps.location.lat() - position[number][0]; // Difference between your and next location in Latitude
		LONGDifference = (gps.location.lng() - position[number][1]) * (90.0 / gps.location.lat()); // Difference between your and next location in Longitude
		disToDes = sqrt(sq(LATDifference) + sq(LONGDifference)) * 65000; // Absolute distance, using Pythagoras
		direction = LATDifference / LONGDifference; // Used to indicate in what direction the next location is
		
		// // Serial prints for debugging purposes
		// Serial.print("Latitude: ");
		// Serial.print(gps.location.lat(), 6); 
		// Serial.print("\t");
		// Serial.print("Longitude: ");
		// Serial.print(gps.location.lng(), 6);
		// Serial.print("\t");
		// Serial.print("Latitude dif: ");
		// Serial.print(LATDifference, 6); 
		// Serial.print("\t");
		// Serial.print("Longitude dif: ");
		// Serial.print(LONGDifference, 6);
		// Serial.print("\t");
		// Serial.print("Distance: ");
		// Serial.print(disToDes);
		// Serial.println("");

		if(gamePhase == 2) // When gyrogame is playing
		{
			lcd.clear();
			if(rememberState) // Distance is only shown if gyro is not tilted too far in gyro game
			{
				lcd.setCursor(0, 1);
				lcd.print("D:");
				lcd.print(disToDes);
				ShowDirection(9, 1);
			}

			if(disToDes < 5) // You first have to walk to the desired loation, before the real gyro game starts
			{
				if(notYetAtStartGyroGame) // When at 1st location | setup for gyrogame happens
				{
					timeTillGyrogameEnds = millis() + 300000;
					for(byte i = 0; i < locationAmount; i++) 
					{
						if(passwordCorrect[i] == 1) { passwordScore++; }
					}
					passwordScore++;
					notYetAtStartGyroGame = false;
				}
				else // You got to the end
				{
					gyrogameFinished = true;
					timeScore = gyroTimerOnLcd;
				}
			}
		}
		else // When first 2 phases are playing | actual GeoCaching
		{
			lcd.clear();
			lcd.home();
			lcd.print("D:");
			lcd.print(disToDes);
			ShowDirection(13, 0);

			if(disToDes < 5)
			{
				onDestination = true;
				lcd.clear();
			}
		}
	}
	clearData();
}



/*
	Voor password:
	//__ 3 pogingen, na 3 pogingen 	-->		password fout, LED wordt rood
									-->		password goed, LED wordt blauw
		 ga naar volgende punt

	//__ Meer LEDs toevoegen

	//__ Wachtwoord in plaats van button voor phase
		 bij punt 6 en 12 code invoeren om verder te kunnen gaan met het spel

	//__ button PMMode langer (15s)

	//__ bij te ver hellen 			--> 	"Danger!", 1x een punt eraf
	//__ na 4-5 seconden 			--> 	door gaan met het spel/rekenen

	Nog niet getest -->
	//__ met phase 3 (gyro game) naar punt lopen
	//__ bij punt aangekomen 			--> 	moet timer aflopen, gyro activeren, naar ander punt (The Circle) lopen
		 bij eindpunt "The Circle" 		--> 	timer stoppen
	//__ Final Score laten zien

	Met het GeoCachen:
	//__ Aangeven waar je heen moet: N/E/S/W
*/