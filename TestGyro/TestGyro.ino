//----------------------------------------------------
// Test Gyroscoop
//----------------------------------------------------
#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
byte pitch, roll;


void setup() {
	Serial.begin(9600);
	Wire.begin();
	setupMPU();
}

void loop() {
	recordAccelRegisters();
	printData();
	delay(100);
}

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

	pitch = atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI;
	roll = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI;
}
void printData() {
	Serial.print(" Accel (g)");
	Serial.print(" X= \t");
	Serial.print(gForceX);
	Serial.print(" Y= \t");
	Serial.print(gForceY);
	Serial.print(" Z= \t");
	Serial.print(gForceZ);
	Serial.print(" Pitch= \t");
	Serial.print(pitch);
	Serial.print(" Roll= \t");
	Serial.println(roll);
}

//**************************************************
//**************************************************

#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;
byte pitch, roll = 0;

void setup() {}


void loop() {
	recordAccelRegisters();
	// recordGyroRegisters();
	printData();
	delay(100);
}

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
	Wire.requestFrom(0b1101000, 6); //Request Accel Registers (3B - 40)
	while(Wire.available() < 6);
	accelX = Wire.read() <<8 | Wire.read(); //Store first two bytes into accelX
	accelY = Wire.read() <<8 | Wire.read(); //Store middle two bytes into accelY
	accelZ = Wire.read() <<8 | Wire.read(); //Store last two bytes into accelZ
	processAccelData();
}

void processAccelData(){
	gForceX = accelX / 16384.0;
	gForceY = accelY / 16384.0; 
	gForceZ = accelZ / 16384.0;

	// Calculating the pitch (rotation around Y-axis) and roll (rotation around X-axis)
	pitch = atan(-1 * accelX / sqrt(pow(accelY, 2) + pow(accelZ, 2))) * 180 / PI;
	roll = atan(-1 * accelY / sqrt(pow(accelX, 2) + pow(accelZ, 2))) * 180 / PI;
}

// void recordGyroRegisters() {
// 	Wire.beginTransmission(0b1101000); //I2C address of the MPU
// 	Wire.write(0x43); //Starting register for Gyro Readings
// 	Wire.endTransmission();
// 	Wire.requestFrom(0b1101000, 6); //Request Gyro Registers (43 - 48)
// 	while(Wire.available() < 6);
// 	gyroX = Wire.read() <<8 | Wire.read(); //Store first two bytes into accelX
// 	gyroY = Wire.read() <<8 | Wire.read(); //Store middle two bytes into accelY
// 	gyroZ = Wire.read() <<8 | Wire.read(); //Store last two bytes into accelZ
// 	processGyroData();
// }

// void processGyroData() {
// 	rotX = gyroX / 131.0;
// 	rotY = gyroY / 131.0; 
// 	rotZ = gyroZ / 131.0;
// }

void printData() {
	// Serial.print("Gyro (deg)");
	// Serial.print(" X= \t");
	// Serial.print(rotX);
	// Serial.print(" Y= \t");
	// Serial.print(rotY);
	// Serial.print(" Z= \t");
	// Serial.print(rotZ);
	Serial.print("Accel (g): ");
	Serial.print("\t X= ");
	Serial.print(gForceX);
	Serial.print("\t Y= ");
	Serial.print(gForceY);
	Serial.print("\t Z= ");
	Serial.print(gForceZ);

	Serial.print("\tPitch= ");
	Serial.print(abs(pitch));
	Serial.print("\tRoll= ");
	Serial.println(abs(roll));
}