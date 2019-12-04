//----------------------------------------------------
// Test Gyroscoop
//----------------------------------------------------
// Include Gyroscope
#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
int pitch, roll;
int tiltFactor;


// Include FastLED
#include "FastLED.h"

#define PIN A5
#define NUM_LEDS 6
CRGB leds[NUM_LEDS];


// Setup
void setup() {
	// Initialize LED's
	FastLED.addLeds<WS2812, PIN, RGB>(leds, NUM_LEDS);

	// Initialize Gyro
	Serial.begin(9600);
	Wire.begin();
	setupMPU();
}

// Loop
void loop() {
	recordAccelRegisters();
	printData();

	for(int i = 0; i < NUM_LEDS; i++)
	{
		// if(i < tiltFactor)
		// {
		// 	leds[i].setRGB(0, 100, 255);
		// }
		// else
		// {
		// 	leds[i].setRGB(0, 0, 0);
		// }
		leds[i].setRGB(0, 0, 255);
	}
	FastLED.show();

	delay(100);
}

// Functions
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

	pitch = abs(atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI);
	roll = abs(atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI);
	tiltFactor = map(pitch, 0, 20, 0, 6);
}
void printData() {
	// Serial.print(" Accel (g)");
	// Serial.print(" X= \t");
	// Serial.print(gForceX);
	// Serial.print(" Y= \t");
	// Serial.print(gForceY);
	// Serial.print(" Z= \t");
	// Serial.print(gForceZ);
	Serial.print(" Pitch= \t");
	Serial.print(pitch);
	Serial.print(" Roll= \t");
	Serial.print(roll);
	Serial.print(" tiltFactor= \t");
	Serial.println(tiltFactor);
}