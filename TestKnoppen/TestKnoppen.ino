//----------------------------------------------------
// Test knoppen voor Arduino
//----------------------------------------------------

// Knop
byte knop = 0;
long knopIngedrukt = 0;
int knopStatus = 0;

void setup() 
{
	// Initialize Serial
	Serial.begin(9600);

	// Button for ProgrammerMode
	pinMode(6, INPUT);
}

void loop()
{
	// Code for button pressed, checking if button is pressed long enough
	knop = digitalRead(6);

	if(knop != knopStatus) 
	{
		knopStatus = knop;
		if(knop == 1) 
		{
			knopIngedrukt = millis();
		} 
		else 
		{
			if(millis() - knopIngedrukt > 1000) 
			{
				PRINT(millis() - knopIngedrukt);
				knopIngedrukt = 0;
			}
		}
	}
}