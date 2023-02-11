String temp = "";
String psswd = "1234";
/*************************(Importieren der genutzten Bibliotheken)*************************/
#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>

/****************************(Definieren der Globale Variablen)***************************/
#define I2CADDR 0x20  // I2C Adresse vom PCF8574

const byte anzZeilen = 4;   //Anzahl Zeilen
const byte anzSpalten = 4;  //Anzahl Spalten

//Hier kannst du die Symbole von deinem Keypad eintragen
char tastenLayout[anzZeilen][anzSpalten] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

//Hier definieren wie das Keypad mit den IO Pins vom PCF8574 verdrahtet ist.
byte zeilenPins[anzZeilen] = { 0, 1, 2, 3 };    //Zeilen Pins
byte spaltenPins[anzSpalten] = { 4, 5, 6, 7 };  //Spalten Pins

//Initialisierung von Keypad
Keypad_I2C i2cKeypad(makeKeymap(tastenLayout), zeilenPins, spaltenPins, anzZeilen, anzSpalten, I2CADDR);

/*******************************************(setup)****************************************/
void setup() {
  Wire.begin();
  i2cKeypad.begin();
  Serial.begin(115200);

  while(true){
    Keypadcompare();
    yield();
  }
}

/*************************************(Hauptprogramm)**************************************/
void loop() {
  
}

bool Keypadcompare() {
  char ausgeleseneTaste = i2cKeypad.getKey();

  if (ausgeleseneTaste != NO_KEY) {
    if (ausgeleseneTaste == '#') {
      if (temp == psswd) {
        Serial.println("Richtiger Pin!");
        temp = "";
      return true;
      } else {
        Serial.println("Falscher Pin!");
        temp = "";
      }
    } else if (ausgeleseneTaste == '*') {
      Serial.println("temporärer Speicher geleert!");
      temp = "";
    } else {
      temp += ausgeleseneTaste;
      if (temp.length() > 6) {
        Serial.println("Zu großer Pin! Bitte nochmal versuchen.");
        temp = "";
      } else {
        Serial.println(temp);
      }
    }
  }
  return false;
}

bool Keypadnewpsswd() {
  char ausgeleseneTaste = i2cKeypad.getKey();

  if (ausgeleseneTaste != NO_KEY) {
    
    if (ausgeleseneTaste == '#') {
      if (temp.length() > 6) {
        Serial.println("Zu großer Pin! Bitte nochmal versuchen.");
        temp = "";
      } else {
        psswd == temp;
        temp = "";
        Serial.println("Passwort gespeichert!");
        return true;
      }

    } else if (ausgeleseneTaste == '*') {
      Serial.println("temporärer Speicher geleert!");
      temp = "";
    } else {
      temp += ausgeleseneTaste;
      if (temp.length() > 6) {
        Serial.println("Zu großer Pin! Bitte nochmal versuchen.");
        temp = "";
      } else {
        Serial.println(temp);
      }
    }
  }
  return false;
}
