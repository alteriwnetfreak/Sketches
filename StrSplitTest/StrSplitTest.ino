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
    for(int n = 0; n < index; n++) {
        Serial.println(strings[n]);
    }
}

void loop()
{
    // put your main code here, to run repeatedly:

}