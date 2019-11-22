//----------------------------------------------------
// Keypad test
//----------------------------------------------------

// Including keypad
#include <Keypad.h>

const byte ROWS = 4; 
const byte COLS = 3; 

char numPad[ROWS][COLS] = {
	{ '1', '2', '3' },
	{ '4', '5', '6' },
	{ '7', '8', '9' },
	{ '.', '0', '#' }
};
//On keypad-board, from left to right
//Connect to pins: 7 to 13
//   -->      /////// /
//          -------------
//          | 1   2   3 |
//          |           |
//          | 4   5   6 |
//          |           |
//          | 7   8   9 |
//          |           |
//          | *   0   # |
//          -------------
// // DIGITAL
// byte colPins[COLS] = { 7, 8, 9 };
// byte rowPins[ROWS] = { 10, 11, 12, 13 };
// // "ANALOG":
byte colPins[COLS] = { 2, 5, 7 };
byte rowPins[ROWS] = { A3, A2, A1, A0 };

Keypad customKeypad = Keypad(makeKeymap(numPad), rowPins, colPins, ROWS, COLS);


// Global Variables | Keypad
#define passwordLength 6 // min 1, wegens "closing char": (\n)
#define latlngAmount 12
#define latCOsize 10 // min 1, wegens "closing char": (\n)
#define lngCOsize 9 // min 1, wegens "closing char": (\n)
byte dataCount = 0;

char data[passwordLength] = "";
char passWord[latlngAmount][passwordLength];
char programmerMode[passwordLength] = "2#111";

// Coordinaten Latitude
char COdata[latCOsize] = "";
float latlngCO[latlngAmount][2];
float newCO;


// Include LCD
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 13, en = 12, d4 = 11, d5 = 10, d6 = 9, d7 = 8; // Digital pins
//const int rs = A5, en = A4, d4 = A3, d5 = A2, d6 = A1, d7 = A0; // Analog pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


// Include EEPROM
#include <EEPROM.h>

int sizeCO, sizePass;

float valueF;
char valueC[6];


// Variabelen voor ProgrammerMode
byte pmSwitch = 0;
bool pmMode = false;

// Knop
long knopIngedrukt = 0;
int knopStatus = 0;

// String Split
char *COposition[6];
int COpositionConv[2];
char *ptr = NULL;

byte index = 0;

// SETUP
void setup() {
	Serial.begin(9600);
	EEPROM_read();
	
	pinMode(6, INPUT);
	
	lcd.begin(16, 2);
	lcd.clear();
	lcd.home();
	lcd.print("Voer wachtwoord in: ");

	Serial.println("Voer wachtwoord in: ");

}

// LOOP
void loop(){
	int knop = digitalRead(6);

	if(knop != knopStatus) {
		knopStatus = knop;
		if(knop == 1) {
			knopIngedrukt = millis();
		} else {
			if(millis() - knopIngedrukt >= 1000) {
				knopIngedrukt = 0;

				if(pmSwitch == 0) {
					lcd.clear();
					lcd.home();
					lcd.print("pmMode. geef ww:");
					
					Serial.println("We switchen naar programmerMode");
					Serial.println("Vul wachtwoord in:");
					
					pmSwitch = 1;
				} else if(pmSwitch == 1) {
					lcd.clear();
					lcd.home();
					lcd.print("Terug naar spel!");

					Serial.println("En terug naar het spel!");

					pmSwitch = 0;
					pmMode = false;
				}
			}
		}
	}

	if(!pmMode) {
		giveData();
		if(dataCount == passwordLength - 1) {
			if(pmSwitch == 0) {
				if(!strcmp(data, passWord[1])) {
					lcd.clear();
					lcd.home();
					lcd.print("Correct!");

					Serial.println("Correct!");
				} else {
					lcd.clear();
					lcd.home();
					lcd.print("Incorrect!");

					Serial.println("Incorrect!");
				}
				clearData();
			} else if(pmSwitch == 1) {	
				if(!strcmp(data, programmerMode)) {
					Serial.println("Old Coördinates: ");
					for(byte i = 0; i < latlngAmount; i++) {
						Serial.print("LAT ");
						Serial.print(i + 1);
						Serial.print(":\t");
						Serial.print(latlngCO[i][0], 6);
						Serial.print("\tLNG ");
						Serial.print(i + 1);
						Serial.print(":\t");
						Serial.print(latlngCO[i][1], 6);
						Serial.print("\tPass ");
						Serial.print(i + 1);
						Serial.print(": ");
						Serial.println(passWord[i]);
					}
					Serial.println("Welk coördinaat wil je veranderen?: ");
					Serial.println("*Doe het in het format xx#yy, waar xx het punt is en yy de LAT, LONG of Pass*");

					lcd.clear();
					lcd.home();
					lcd.print("Welk coordinaat?");

					pmSwitch++;
				} else {
					lcd.clear();
					lcd.home();
					lcd.print("Onjuist!");
					
					Serial.println("Onjuist!!!!!!!!!!!");
					pmSwitch = 0;
				}
				clearData();
			} else {    
				ptr = strtok(data, "#");
				while(ptr != NULL)
			    {
			        COposition[index] = ptr;
			        index++;
			        ptr = strtok(NULL, "#");  // takes a list of delimiters
			    }
				for(int n = 0; n < index; n++) {
			        COpositionConv[n] = atoi(COposition[n]);
			        Serial.println(COpositionConv[n]);
			    }
				index = 0;

			    Serial.print("Positie: ");
			    Serial.print(COpositionConv[0]);
			    Serial.print("\t");
			    Serial.print("LAT/LONG: ");
			    Serial.print(COpositionConv[1]);
			    Serial.println("");
				
				if(COpositionConv[0] > latlngAmount || COpositionConv[1] > 3) {
					lcd.clear();
				    lcd.home();
				    lcd.print("Niet Geldig!");

				    Serial.println("Niet Geldig!");

				    // pmSwitch = 1;
				} else {
					Serial.println("Geef een coördinaat/pass: ");

					lcd.clear();
					lcd.home();
					lcd.print("Geef CO/PassW:");
					pmMode = !pmMode;
				}
				clearData();
			}
		}
	} else {
		giveCoordinate();
		if(COpositionConv[1] == 1) {
			if(dataCount == latCOsize - 1) {
				sizeCO = COpositionConv[0] * (2*sizeof(float));
				EEPROM.put(sizeCO, atof(COdata));
				latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1] = EEPROM.get(sizeCO, valueF);

				lcd.clear();
				lcd.home();
				lcd.print("New LAT:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1], 6);

				Serial.print("New Coördinate(s) and Pass: ");
				Serial.println(latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1], 6);
				Serial.println("");
				for(byte i = 0; i < latlngAmount; i++) {
					Serial.print("LAT: ");
					Serial.print(latlngCO[i][0], 6);
					Serial.print("\tLNG: ");
					Serial.print(latlngCO[i][1], 6);
					Serial.print("\tPass: ");
					Serial.println(passWord[i]);
				}
				clearData();
				pmMode = !pmMode;
			}
		} else if(COpositionConv[1] == 2) {
			if(dataCount == lngCOsize - 1) {
				sizeCO = COpositionConv[0] * (2*sizeof(float));
				EEPROM.put(sizeCO + 4, atof(COdata));
				latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1] = EEPROM.get(sizeCO + 4, valueF);

				lcd.clear();
				lcd.home();
				lcd.print("New LONG:");
				lcd.setCursor(0, 1);
				lcd.print(latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1], 6);

				Serial.print("New Coördinate(s) and Pass: ");
				Serial.println(latlngCO[COpositionConv[0] - 1][COpositionConv[1] - 1], 6);
				Serial.println("");
				for(byte i = 0; i < latlngAmount; i++) {
					Serial.print("LAT: ");
					Serial.print(latlngCO[i][0], 6);
					Serial.print("\tLNG: ");
					Serial.print(latlngCO[i][1], 6);
					Serial.print("\tPass: ");
					Serial.println(passWord[i]);
				}
				clearData();
				pmMode = !pmMode;
			}
		} else {
			if(dataCount == passwordLength - 1) {
				sizePass = (COpositionConv[0] - 1) * (7*sizeof(char)) + sizeof(latlngCO);
				EEPROM.put(sizePass, COdata);
				memcpy(
					passWord[COpositionConv[0] - 1], 
					EEPROM.get(sizePass, valueC), 
					sizeof(passWord[COpositionConv[0] - 1])
				);
				// passWord[COpositionConv[0] - 1] = EEPROM.get(sizePass, valueC);

				Serial.print("size of passWord: ");
				Serial.print(sizeof(passWord[COpositionConv[0] - 1]));
				Serial.print("\tvalueC: ");
				Serial.print(valueC);
				Serial.print("\tsizeCO: ");
				Serial.print(sizePass);
				Serial.print("\tCOpositionConv: ");
				Serial.println(COpositionConv[0] - 1);

				lcd.clear();
				lcd.home();
				lcd.print("New Pass:");
				lcd.setCursor(0, 1);
				lcd.print(passWord[COpositionConv[0] - 1]);

				Serial.print("New Coördinate(s) and Pass: ");
				Serial.println(passWord[COpositionConv[0] - 1]);
				Serial.println("");
				for(byte i = 0; i < latlngAmount; i++) {
					Serial.print("LAT: ");
					Serial.print(latlngCO[i][0], 6);
					Serial.print("\tLNG: ");
					Serial.print(latlngCO[i][1], 6);
					Serial.print("\tPass: ");
					Serial.println(passWord[i]);
				}
				clearData();
				pmMode = !pmMode;
			}
		}
	}
}

char* giveData() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		data[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(data);
		Serial.println(data);
	}
	return data;
}
char* giveCoordinate() {
	char customKey = customKeypad.getKey();
	if(customKey) {
		COdata[dataCount] = customKey;
		dataCount++;
		lcd.setCursor(0, 1);
		lcd.print(COdata);
		Serial.println(COdata);
	}
	return COdata;
}
void clearData() {
	for(byte i = 0; i < passwordLength; i++) {
		data[i] = 0;
	}
	for(byte i = 0; i < latCOsize; i++) {
		COdata[i] = 0;
	}
	dataCount = 0;
}


void EEPROM_read() {
	for(byte i = 0; i < latlngAmount; i++) {
		sizeCO = i * (2*sizeof(float));

		latlngCO[i][0] = EEPROM.get(sizeCO, valueF);

		Serial.print(sizeof(latlngCO[i]));
		Serial.print("\t");
		Serial.print(sizeCO);
		Serial.print("\t");
		Serial.print(valueF);
		Serial.print("\t\t");


		latlngCO[i][1] = EEPROM.get(sizeCO + 4, valueF);

		Serial.print(sizeof(latlngCO[i]));
		Serial.print("\t");
		Serial.print(sizeCO + 4);
		Serial.print("\t");
		Serial.println(valueF);
	}

	for(byte o = 0; o < latlngAmount; o++) {
		sizePass = o * (7*sizeof(char)) + sizeof(latlngCO);
		
		memcpy(passWord[o], EEPROM.get(sizePass, valueC), sizeof(passWord[o]));

		Serial.print(sizePass);
		Serial.print("\t");
		Serial.print(o * (7*sizeof(char)));
		Serial.print("\t");
		Serial.print(sizeof(latlngCO));
		Serial.print("\t");
		Serial.println(valueC);

	}
	Serial.println("");
	Serial.println("");
}
// void EEPROM_write() {
// 	for(byte i = 0; i < latlngAmount; i++) {
// 		sizeCO = i * (2*sizeof(float));

// 		EEPROM.put(sizeCO, latlngCO[i][0]);
// 		EEPROM.put(sizeCO + 4, latlngCO[i][1]);
// 		Serial.print(sizeCO);
// 		Serial.print("\t");
// 		Serial.println(sizeCO + 4);
// 	}

// 	Serial.println("");

// 	for(byte i = 0; i < latlngAmount; i++) {
// 		sizePass = i * (6*sizeof(char)) + sizeof(latlngCO);
		
// 		EEPROM.put(sizePass, passWord[i]);
// 		Serial.print(sizePass);
// 		Serial.print("\t");
// 	}
// 	Serial.println("");
// }
/*
*/