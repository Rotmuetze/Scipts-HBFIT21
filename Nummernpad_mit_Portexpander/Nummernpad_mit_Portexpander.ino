String temp = "";
String psswd = "1234";


#include <Keypad_I2C.h>
#include <Keypad.h>
#include <Wire.h>


#define I2CADDR 0x20

const byte anzZeilen = 4;
const byte anzSpalten = 4;


char tastenLayout[anzZeilen][anzSpalten] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};


byte zeilenPins[anzZeilen] = { 0, 1, 2, 3 };
byte spaltenPins[anzSpalten] = { 4, 5, 6, 7 };


Keypad_I2C i2cKeypad(makeKeymap(tastenLayout), zeilenPins, spaltenPins, anzZeilen, anzSpalten, I2CADDR);

void setup() {
  Wire.begin();
  i2cKeypad.begin();
  Serial.begin(115200);

  while(true){
    Keypadcompare();
    yield();
  }
}

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
