//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

// #include <I2Cdev.h>
// #include <MPU6050_6Axis_MotionApps_V6_12.h>
#include <Wire.h>

const int MPU = 0x68; // I2C address of the MPU6050 accelerometer

int16_t AcX, AcY, AcZ;
int axis = 0;
int angle = 0;
int pitch = 0;

void setup() {
	// Initialize interface to the MPU6050
	Wire.begin();
	Wire.beginTransmission(MPU);
	Wire.write(0x6B);
	Wire.write(0);
	Wire.endTransmission(true);

	Serial.begin(19200);
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

	// Calculating the Pitch angle (rotation around Y-axis)
	angle = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
	
	pitch = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;

	Serial.print("Angle: ");
	Serial.print(abs(angle));
	Serial.print("     ");
	Serial.print("deg");
	Serial.print("\t");

	Serial.print("Pitch: ");
	Serial.print(abs(pitch));
	Serial.print("     ");
	Serial.println("deg");
}