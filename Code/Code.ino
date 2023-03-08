//Bild Import
#include "schluessel.h"

#include "psswd.h"

//Variablen
String temp = "";
String keypadpsswd = "";
String rfidpsswd = "";
String psswdcoded;

//Webserver
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);


//Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//KeypadI2C
#include <Keypad_I2C.h>
#include <Keypad.h>
//#include <Wire.h>
char tastenLayout[4][4] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte zeilenPins[4] = { 0, 1, 2, 3 };
byte spaltenPins[4] = { 4, 5, 6, 7 };
Keypad_I2C i2cKeypad(makeKeymap(tastenLayout), zeilenPins, spaltenPins, 4, 4, 0x20);


//RFID
#include <SPI.h>
#include <MFRC522.h>
MFRC522 mfrc522(0, 1);

//Huskylens
#include "HUSKYLENS.h"
HUSKYLENS huskylens;


//Fingerprintsensor
#include <Adafruit_Fingerprint.h>
SoftwareSerial mySerial(2, 3);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);



void setup() {
  Serial.begin(115200);
  Serial.println();

  //Websever
  WiFi.softAP(apssid,appsswd);
  Serial.print("AP IP address: ");
  String APIP = WiFi.softAPIP().toString();
  Serial.println(APIP);
  server.on("/", handleRoot);
  server.onNotFound(handleRoot);
  server.begin();

  //Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    for (;;)
      ;
  }

  //KeypadI2C
  Wire.begin();
  i2cKeypad.begin();

  //RFID
  SPI.begin();
  mfrc522.PCD_Init();

  delay(2000);  //Den Komponenten Zeit zum hochfahren geben
  //Huskylens
  if (!huskylens.begin(Wire)) {
    huskylens.begin(Wire);
    displaytext("Huskylens nicht gefunden", 0);
  }
  Serial.println("I2C Verbindung erfolgreich hergestellt.");
  huskylens.writeAlgorithm(ALGORITHM_FACE_RECOGNITION);

  //Fingerprint
  finger.begin(57600);
  if (finger.verifyPassword()) {
  } else {
    displaytext("Fingerabdruck nicht gefunden", 3000);
  }
  startseite();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  temp = "";
  String back = server.arg("authentication_method");
  String newdata = server.arg("newData");
  Serial.println(back + newdata);
  sendroot();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 20);
  if (back == "rfid") {
    if (newdata == "newdata") {
      temp = "";
      display.setCursor(0, 20);
      display.clearDisplay();
      display.println("Warte auf Tag zum einspeichern:");
      display.display();
      while (!RFIDnewpsswd()) {
        yield();
      }
    } else {
      display.setCursor(0, 20);
      display.clearDisplay();
      display.println("Warte auf Tag:");
      display.display();
      while (!RFIDcompare()) {
        yield();
      }
    }
    startseite();
  }
  if (back == "fingerprint") {
    if (newdata == "newdata") {
      while (!FingerprintEnroll()) {
        yield();
      }
    } else {
      while (!Fingerprintcompare()) {
        yield();
      }
    }
    startseite();
  }
  if (back == "faceid") {
    if (newdata == "newdata") {
      while (!huskylensnewface()) {
        yield();
      }
    } else {
      while (!huskylensfacecompare()) {
        yield();
      }
    }
    startseite();
  }
  if (back == "numfield") {
    if (newdata == "newdata") {
      display.clearDisplay();
      display.println("Neuen Pin eintragen:");
      display.println(psswdcoded);
      display.display();
      while (!Keypadnewpsswd()) {
        yield();
      }
    } else {
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("Pin:");
      display.println(psswdcoded);
      display.display();
      while (!Keypadcompare()) {
        yield();
      }
    }
    startseite();
  }
}

void sendroot() {
  String message = "<head><meta name=viewport content=width=device-width, initial-scale=1.0, user-scalable=no></head><body><form action=/ method=post><input type=radio id=numfield name=authentication_method value=numfield><label for=numfield>Nummernfeld</label><br><input type=radio id=rfid name=authentication_method value=rfid><label for=rfid>RFID</label><br><input type=radio id=faceid name=authentication_method value=faceid><label for=faceid>Face-ID</label><br><input type=radio id=fingerprint name=authentication_method value=fingerprint><label for=fingerprint>Fingerabdrucksensor</label><br><br><br><input type=radio id=newData name=newData value=newdata>Neues Passwort vergeben?<label for=newData></label><br><br><input type=radio id=newData name=newData value=nichtnutzen checked=checked>Passwort &Uuml;berpr&uuml;fen<label for=nichtnutzen!></label><br><br><input type=submit value=&Uuml;bernehmen></form></body>";

  server.send(200, "text/html", message);
}

void startseite() {
  display.clearDisplay();
  display.drawXBitmap(0, 0, png_transparent_computer_icons_key_angle_desktop_wallpaper_old_k_bits, 56, 64, WHITE);
  display.setCursor(60, 20);
  display.setTextColor(WHITE);
  display.println("Authenti-");
  display.setCursor(60, 30);
  display.println("fizierungs-");
  display.setCursor(60, 40);
  display.println("methoden");
  display.setCursor(30, 54);
  display.println("IP: " + WiFi.softAPIP().toString());
  display.display();
}

void displaytext(String text, long delaylong) {
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(text);
  display.display();
  delay(delaylong);
}

bool Keypadcompare() {
  if (keypadpsswd == "") {
    display.setCursor(0, 20);
    display.clearDisplay();
    display.println("Zuerst Pin speichern!");
    display.display();
    delay(3000);
    startseite();
    return true;
  }
  char ausgeleseneTaste = i2cKeypad.getKey();

  if (ausgeleseneTaste != NO_KEY) {
    if (ausgeleseneTaste == '#') {
      if (temp == keypadpsswd) {
        psswdcoded.clear();
        temp = "";
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Pin richtig!");
        display.display();
        delay(2000);
        startseite();
        return true;
      } else {
        psswdcoded.clear();
        temp = "";
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Pin falsch!");
        display.display();
        startseite();
        delay(2000);
        return true;
      }
    } else if (ausgeleseneTaste == '*') {
      psswdcoded.clear();
      temp = "";
    } else {
      psswdcoded += "*";
      temp += ausgeleseneTaste;
      if (temp.length() > 6) {
        temp = "";
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Zu gross! Nochmal versuchen");
        display.display();
        delay(1000);
      }
    }
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("PIN:");
    display.setTextSize(2);
    display.println(psswdcoded);
    display.setTextSize(1);
    display.display();
  }
  return false;
}

bool Keypadnewpsswd() {
  char ausgeleseneTaste = i2cKeypad.getKey();

  if (ausgeleseneTaste != NO_KEY) {

    if (ausgeleseneTaste == '#') {
       if (temp.length() < 4) {
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Zu klein! Nochmal versuchen");
        psswdcoded.clear();
        temp = "";
        display.display();
        delay(1000);

      } else {
        keypadpsswd.clear();
        keypadpsswd += temp;
        if (temp.length() > 6) {
        psswdcoded.clear();
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Zu groß! Nochmal versuchen");
        psswdcoded.clear();
        temp = "";
        display.display();
        delay(1000);
      }
        psswdcoded.clear();
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Pin aktualisiert!");
        display.display();
        delay(1000);
        return true;
      }

    } else if (ausgeleseneTaste == '*') {
      psswdcoded.clear();
      temp = "";
    } else {
      temp += ausgeleseneTaste;
      psswdcoded += "*";


      if (temp.length() > 6) {
        psswdcoded.clear();
        temp = "";
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Zu gross! Nochmal versuchen");
        display.display();
        delay(1000);
      }
    }
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Neuen Pin eintragen:");
    display.setTextSize(2);
    display.println(psswdcoded);
    display.setTextSize(1);
    display.display();
  }
  return false;
}

bool RFIDnewpsswd() {
  display.setCursor(0, 20);
  display.clearDisplay();
  display.println("Warte auf Tag zum einspeichern:");
  display.display();
  if (!mfrc522.PICC_IsNewCardPresent()) {
    delay(200);
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    temp.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    temp.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  temp.toUpperCase();
  rfidpsswd.clear();
  rfidpsswd = temp;
  Serial.println("Tag gespeichert!");
  display.setCursor(0, 20);
  display.clearDisplay();
  display.println("Tag gespeichert!");
  display.display();
  delay(3000);
  return true;
}

bool RFIDcompare() {
  if (rfidpsswd == "") {
    display.setCursor(0, 20);
    display.clearDisplay();
    display.println("Zuerst Tag speichern!");
    display.display();
    delay(3000);
    startseite();
    return true;
  }
  display.setCursor(0, 20);
  display.clearDisplay();
  display.println("Warte auf Tag:");
  display.display();
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    temp.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    temp.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  temp.toUpperCase();
  if (temp == rfidpsswd) {
    Serial.println("richtiger Tag");
    display.setCursor(0, 20);
    display.clearDisplay();
    display.println("Richtiger Tag");
    display.display();
    delay(1000);
    startseite();
  } else {
    Serial.println("falscher Tag");
    display.setCursor(0, 20);
    display.clearDisplay();
    display.println("Falscher Tag");
    display.print("Nochmal probieren!");
    display.display();
    delay(1000);
  }
  return true;
}

bool huskylensnewface() {
  if (huskylens.request()) {
    HUSKYLENSResult result = huskylens.read();
    if (huskylens.isLearned()) {
      for (int i = 10; i > 0; i--) {  // Countdown
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Knopf zwei mal");
        display.print("dr");
        display.print("\x81");
        display.print("cken");
        String s = String(i);
        display.setCursor(0, 48);
        display.println("         Weiter in " + s);
        display.display();
        delay(1000);
      }
    }
    for (int i = 10; i > 0; i--) {
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("Knopf 2 sekunden");
      display.print("halten bis gr");
      display.print("\x81");
      display.println("nes");
      display.println("Licht leuchtet");
      String s = String(i);
      display.setCursor(0, 48);
      display.println("         Weiter in " + s);
      display.display();
      delay(1000);
    }
    return true;
  }
  return false;
}

bool huskylensfacecompare() {
  if (huskylens.request()) {
    if (huskylens.isLearned()) {
      if (huskylens.available() > 0) {
        HUSKYLENSResult result = huskylens.read();
        if (result.ID == 1) {
          display.clearDisplay();
          display.setCursor(0, 20);
          display.println("Person erkannt!");
          display.println("Richtige Person!");
          display.display();
          delay(3000);
          return true;
        } else {
          display.clearDisplay();
          display.setCursor(0, 20);
          display.println("Person erkannt!");
          display.println("Falsche Person!");
          display.display();
          delay(3000);
          return true;
        }
      } else {
        display.clearDisplay();
        display.setCursor(0, 20);
        display.println("Vor Kamera treten!");
        display.display();
        delay(100);
        return false;
      }
    } else if (huskylens.available() == 0) {
      display.clearDisplay();
      display.setCursor(0, 20);
      display.println("Kamera anlernen!");
      display.display();
      delay(1000);
      return true;
    }
  }
  return false;
}

bool FingerprintEnroll() {
  int id = 1;
  int p = -1;

  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        displaytext("Finger erkannt!", 0);
        break;
      case FINGERPRINT_NOFINGER:
        displaytext("Finger auflegen!", 0);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        break;
      default:
        displaytext("Unbekannter Fehler!", 2000);
        startseite();
        return true;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      displaytext("Bild konvertiert", 0);
      break;
    case FINGERPRINT_IMAGEMESS:
      displaytext("Zu undeutiges Bild!", 2000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    default:
      displaytext("Unbekannter Fehler!", 2000);
      startseite();
      return true;
  }

  displaytext("Finger entfernen!", 2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        displaytext("Finger erkannt!", 0);
        break;
      case FINGERPRINT_NOFINGER:
        displaytext("Gleichen Finger auflegen!", 0);
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        break;
      default:
        displaytext("Unbekannter Fehler", 2000);
        startseite();
        return true;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      displaytext("Bild konvertiert", 0);
      break;
    case FINGERPRINT_IMAGEMESS:
      displaytext("Zu undeutiges Bild!", 2000);
      return false;
    case FINGERPRINT_PACKETRECIEVEERR:
      return p;
    default:
      displaytext("Unbekannter Fehler!", 2000);
      startseite();
      return true;
  }

  // OK converted!
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    displaytext("Fingerabruck erstellt!", 0);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.print("Unterschiedliche Fingerabdr");
    display.print("\x82");
    display.print("cke!");
    display.display();
    delay(5000);
    return true;
  } else {
    displaytext("Unbekannter Fehler!", 2000);
    startseite();
    return true;
  }

  p = finger.storeModel(1);
  if (p == FINGERPRINT_OK) {
    displaytext("Gespeichert!", 1000);
    displaytext("Finger entfernen", 2000);
    return true;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    return false;
  } else {
    displaytext("Unbekannter Fehler!", 2000);
    startseite();
    return true;
  }
}

bool Fingerprintcompare() {
  displaytext("Finger auflegen!", 0);
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return false;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return false;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return false;

  display.setCursor(0, 20);
  display.clearDisplay();
  display.println("Finger erkannt");
  double dtext = (double)finger.confidence / 2.55;
  String text = String(dtext) + "%";
  display.print("" + text);
  display.display();
  delay(3000);
  return true;
  ;
}