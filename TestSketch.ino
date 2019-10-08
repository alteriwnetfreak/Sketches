void setup() {
  // put your setup code here, to run once:
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);

  pinMode(A0, INPUT);

  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int iets = analogRead(A0);
  digitalWrite(13, HIGH);
  digitalWrite(12, HIGH);
  
  delay(iets);

  digitalWrite(13, LOW);
  Serial.print("Sensor waarde: ");
  Serial.println(iets);

  delay(iets);

  digitalWrite(13, HIGH);
  digitalWrite(12, LOW);
  
  delay(iets);

  digitalWrite(13, LOW);
  Serial.print("Sensor waarde: ");
  Serial.println(iets);

  delay(iets);
}
