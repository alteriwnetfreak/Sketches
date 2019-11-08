//----------------------------------------------------
// Test Digital LED's
//----------------------------------------------------

// #include <Wire.h>

// const int MPU = 0x68; // I2C address of the MPU6050 accelerometer

// int16_t AcX, AcY, AcZ;
// //int axis = 0;
// int pitch = 0;
// int roll = 0;


//*********************************************
// Include LCD
//*********************************************
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// void setup() {
// 	//Serial.begin(115200);

// 	lcd.begin(16, 2);
// 	lcd.print("Gaat dit goed?");

// 	setupMPU();
// }

// void loop() {
// 	// Read the accelerometer data
// 	Wire.beginTransmission(MPU);
// 	Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
// 	Wire.endTransmission(false);
// 	Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
	
// 	AcX = Wire.read() << 8 | Wire.read(); // X-axis value
// 	AcY = Wire.read() << 8 | Wire.read(); // Y-axis value
// 	AcZ = Wire.read() << 8 | Wire.read(); // Z-axis value

// 	// Calculating the pitch (rotation around Y-axis) and roll (rotation around X-axis)
// 	pitch = atan(-1 * AcX / sqrt(pow(AcY, 2) + pow(AcZ, 2))) * 180 / PI;
// 	roll = atan(-1 * AcY / sqrt(pow(AcX, 2) + pow(AcZ, 2))) * 180 / PI;

// 	// Serial.print("Pitch: ");
// 	// Serial.print(abs(pitch));
// 	// Serial.print(" deg");
	
// 	// Serial.print("\t\t");

// 	// Serial.print("Roll: ");
// 	// Serial.print(abs(roll));
// 	// Serial.println(" deg");
	
// 	// Print naar LCD
// 	lcd.clear();
// 	lcd.home();
// 	lcd.print("Pitch: ");
// 	lcd.print(abs(pitch));

// 	lcd.setCursor(0, 1);
// 	lcd.print("Roll: ");
// 	lcd.print(abs(roll));
// }

/*
===Contact & Support===
Website: http://eeenthusiast.com/
Youtube: https://www.youtube.com/EEEnthusiast
Facebook: https://www.facebook.com/EEEnthusiast/
Patreon: https://www.patreon.com/EE_Enthusiast
Revision: 1.0 (July 13th, 2016)
===Hardware===
- Arduino Uno R3
- MPU-6050 (Available from: http://eeenthusiast.com/product/6dof-mpu-6050-accelerometer-gyroscope-temperature/)
===Software===
- Latest Software: https://github.com/VRomanov89/EEEnthusiast/tree/master/MPU-6050%20Implementation/MPU6050_Implementation
- Arduino IDE v1.6.9
- Arduino Wire library
===Terms of use===
The software is provided by EEEnthusiast without warranty of any kind. In no event shall the authors or 
copyright holders be liable for any claim, damages or other liability, whether in an action of contract, 
tort or otherwise, arising from, out of or in connection with the software or the use or other dealings in 
the software.
*/

#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  setupMPU();
}


void loop() {
  recordAccelRegisters();
  // recordGyroRegisters();
  printData();
  // delay(100);
}

void setupMPU(){
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
}

// void recordGyroRegisters() {
//   Wire.beginTransmission(0b1101000); //I2C address of the MPU
//   Wire.write(0x43); //Starting register for Gyro Readings
//   Wire.endTransmission();
//   Wire.requestFrom(0b1101000,6); //Request Gyro Registers (43 - 48)
//   while(Wire.available() < 6);
//   gyroX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
//   gyroY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
//   gyroZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
//   processGyroData();
// }

// void processGyroData() {
//   rotX = gyroX / 131.0;
//   rotY = gyroY / 131.0; 
//   rotZ = gyroZ / 131.0;
// }

void printData() {
  // Serial.print("Gyro (deg)");
  // Serial.print(" X= \t");
  // Serial.print(rotX);
  // Serial.print(" Y= \t");
  // Serial.print(rotY);
  // Serial.print(" Z= \t");
  // Serial.println(rotZ);
  Serial.print(" Accel (g)");
  Serial.print(" X= \t");
  Serial.print(gForceX);
  Serial.print(" Y= \t");
  Serial.print(gForceY);
  Serial.print(" Z= \t");
  Serial.println(gForceZ);
}