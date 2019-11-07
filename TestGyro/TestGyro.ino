//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

#include <Wire.h>

const int MPU = 0x68; // I2C address of the MPU6050 accelerometer

int16_t AcX, AcY, AcZ;
//int axis = 0;
int pitch = 0;
int roll = 0;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


void setup() {
	lcd.begin(16, 2);
	lcd.print("Gaat dit goed?");

	Serial.begin(9600);
	
	// Initialize interface to the MPU6050
	Wire.begin();
	Wire.beginTransmission(MPU);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);
}

void loop() {
	// Read the accelerometer data
	Wire.beginTransmission(MPU);
	Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
	Wire.endTransmission(false);
	Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
	
	AcX = Wire.read() << 8 | Wire.read(); // X-axis value
	AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
	AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value

	// Calculating the pitch (rotation around Y-axis) and roll (rotation around X-axis)
	pitch = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
	roll = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;

	// Serial.print("Pitch: ");
	// Serial.print(abs(pitch));
	// Serial.print(" deg");
	
	// Serial.print("\t\t");

	// Serial.print("Roll: ");
	// Serial.print(abs(roll));
	// Serial.println(" deg");
	
	// Print naar LCD
	lcd.clear();
	lcd.home();
	lcd.print("Pitch: ");
	lcd.print(abs(pitch));

	lcd.setCursor(0, 1);
	lcd.print("Roll: ");
	lcd.print(abs(roll));
}