//----------------------------------------------------
// Test Gyroscoop
//----------------------------------------------------
// Include Gyroscope
#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
int pitch, roll;
int tiltFactor;
byte tiltMax = 15;


// Include LCD
#include <LiquidCrystal.h>

const byte rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// bytes for custom characters
byte customCharacter[8][8] =
{ 
	{ 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111 },
	{ 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111 }
};


// Include FastLED
#include <FastLED.h>

#define PIN A4
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];


// Setup
void setup() 
{
	// Creating the custom characters
	for(byte i = 0; i < 8; i++)
	{
		lcd.createChar(i, customCharacter[i]);
	}

	// Initialize Serial | LCD
	Serial.begin(9600);
	lcd.begin(16, 2);
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);
	
	for(int i = 0; i < NUM_LEDS; i++)
	{
		leds[i].setRGB(255, 0, 0);
	}
	delay(500);
	for(int i = 0; i < NUM_LEDS; i++)
	{
		leds[i].setRGB(0, 255, 0);
	}
	delay(500);
	for(int i = 0; i < NUM_LEDS; i++)
	{
		leds[i].setRGB(0, 0, 255);
	}

	// Initialize Gyro
	setupMPU();
}

// Loop
void loop() 
{
	recordAccelRegisters();
	printData();
	delay(100);
}

// Functions
// Gyro functions
void setupMPU(){
	Wire.begin();
	Wire.beginTransmission(0x69);
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
void recordAccelRegisters() {
	Wire.beginTransmission(0b1101000); //I2C address of the MPU
	Wire.write(0x3B); //Starting register for Accel Readings
	Wire.endTransmission();
	Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
	while(Wire.available() < 6);
	accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
	accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
	accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
	processAccelData();
}
void processAccelData(){
	gForceX = accelX / 16384.0;
	gForceY = accelY / 16384.0; 
	gForceZ = accelZ / 16384.0;

	pitch = (atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI);
	roll = (atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI);
	tiltFactor = map(roll, -10, 10, -7, 7);
}
void printData() {
	// Serial.print(" Accel (g)");
	// Serial.print(" X= \t");
	// Serial.print(gForceX);
	// Serial.print(" Y= \t");
	// Serial.print(gForceY);
	// Serial.print(" Z= \t");
	// Serial.print(gForceZ);

	Serial.print("Pitch = ");
	Serial.print(pitch);
	Serial.print("\tRoll = ");
	Serial.print(roll);
	Serial.print("\tTilt factor = ");
	Serial.println(tiltFactor);

	lcd.home();
	lcd.print("Level:");
	
	for(int i = 0; i < tiltMax; i++)
	{
		lcd.setCursor(i, 1);
		lcd.write(byte(constrain(abs(tiltFactor - (i - 7)), 0, 7)));
	}

	for(int i = 0; i < NUM_LEDS; i++)
	{
		if(tiltFactor < i)
		{
			leds[i].setRGB(0, 255, 0);
		}
		else
		{
			leds[i].setRGB(0, 0, 255);
		}
	}
}