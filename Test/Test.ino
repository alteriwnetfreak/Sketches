// Test voor de Arduino, en hopelijk zonder toeters en belletjes...

void setup() {
	float angle = 0;

	Serial.begin(9600);
}

void loop() {
	Serial.println(sin(angle));
	angle++;

	if(angle == 90) {
		angle = 0;
	}
}