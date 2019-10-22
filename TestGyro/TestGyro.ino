#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps_V6_12.h"
#include "Wire.h"

MPU6050 mpu;
uint8_t devStatus;	// return status after each device operation (0 = success, !0 = error)

Quaternion q;// [w, x, y, z]: quaternion container
uint8_t fifoBuffer[64]; // FIFO storage buffer

void setup() {
	Serial.begin(9600);

	mpu.initialize();
	devStatus = mpu.dmpInitialize();
	
	mpu.setXGyroOffset(51);
	mpu.setYGyroOffset(8);
	mpu.setZGyroOffset(21);
	mpu.setXAccelOffset(1150);
	mpu.setYAccelOffset(-50);
	mpu.setZAccelOffset(1060);
	
	if (devStatus == 0) {
		mpu.CalibrateAccel(6);
		mpu.CalibrateGyro(6);
		
		mpu.setDMPEnabled(true);
	}
}

void loop() {
	mpu.dmpGetQuaternion(&q, fifoBuffer);
	Serial.print("quat\t");
	Serial.print(q.w);
	Serial.print("\t");
	Serial.print(q.x);
	Serial.print("\t");
	Serial.print(q.y);
	Serial.print("\t");
	Serial.println(q.z);
}
