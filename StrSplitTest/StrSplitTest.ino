char array[] = "AAAA;A1:3;B1:5;ZZZZ";
char *strings[10];
char *ptr = NULL;

void setup()
{
    Serial.begin(9600);
    //Serial.print(array);
    byte index = 0;
    ptr = strtok(array, ":;");  // takes a list of delimiters
    while(ptr != NULL)
    {
        strings[index] = ptr;
        index++;
        ptr = strtok(NULL, ":;");  // takes a list of delimiters
    }
    //Serial.println(index);
    // print the tokens
    for(int n = 0; n < index; n++) 
    {
        Serial.println(strings[n]);
    }
}

void loop()
{
    // put your main code here, to run repeatedly:

}


// // Code van ander dinge
// ptr = strtok(data, "#");
// while(ptr != NULL)
// {
//     COposition[index] = ptr;
//     index++;
//     ptr = strtok(NULL, "#");  // takes a list of delimiters
// }
// for(int n = 0; n < index; n++) {
//     COpositionConv[n] = atoi(COposition[n]);
//     Serial.println(COpositionConv[n]);
// }
// index = 0;

// Serial.println("");
// Serial.print("Positie: ");
// Serial.print(COpositionConv[0]);
// Serial.print("\tLAT/LONG: ");
// Serial.print(COpositionConv[1]);
// Serial.println("");

// if(COpositionConv[0] > latlngAmount || COpositionConv[1] > 2) {
//     lcd.clear();
//     lcd.home();
//     lcd.print("Niet Geldig");
//     lcd.setCursor(0, 1);
//     lcd.print("Probeer nog eens");

//     Serial.println("Niet Geldig, probeer het nog eens");
// } else {
//     lcd.clear();
//     lcd.home();
//     lcd.print("Old LAT:");
//     lcd.setCursor(0, 1);
//     lcd.print(latlngCO[COpositionConv[0]][COpositionConv[1]], 6);

//     Serial.println("Old Coördinates: ");
//     for(byte i = 0; i < latlngAmount; i++) {
        


//         Serial.print("LAT: ");
//         Serial.print(latlngCO[i][0], 6);
//         Serial.print("\tLNG: ");
//         Serial.println(latlngCO[i][1], 6);



//     }
//     pmMode = !pmMode;
// }