/***************************************************
  This is an example sketch for our optical Fingerprint sensor

  Designed specifically to work with the Adafruit BMP085 Breakout
  ----> http://www.adafruit.com/products/751

  These displays use TTL Serial to communicate, 2 pins are required to
  interface
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Adafruit_Fingerprint.h>


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(2, 3);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);



//Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void setup() {

  //Display
  Wire.begin();
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();
  display.display();

  Serial.begin(9600);
  finger.begin(57600);  //baud rate

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
      delay(1);
    }
  }
   while (!getFingerprintEnroll())
    ;
}

void displaytext(String text, long delaylong) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(text);
  display.display();
  delay(delaylong);
}

void loop()  // run over and over again
{
 
}

bool getFingerprintEnroll() {
  int id = 1;
  int p = -1;

  displaytext("Finger auflegen!", 0);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        displaytext("Finger erkannt!", 1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Kommunikation fehlgeschlagen");
        break;
      default:
        displaytext("Unbekannter Fehler!", 2000);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      displaytext("Bild konvertiert", 1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      displaytext("Zu undeutiges Bild!", 2000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Kommunikation fehlgeschlagen");
      return p;
    default:
      displaytext("Unbekannter Fehler!", 2000);
      return p;
  }

  displaytext("Finger entfernen!", 2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  p = -1;
  displaytext("Gleichen Finger auflegen!", 0);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        displaytext("Finger erkannt!", 1000);
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Kommunikation fehlgeschlagen");
        break;
      default:
        displaytext("Unbekannter Fehler", 2000);
        break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      displaytext("Bild konvertiert", 1000);
      break;
    case FINGERPRINT_IMAGEMESS:
      displaytext("Zu undeutiges Bild!", 2000);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Kommunikation fehlgeschlagen");
      return p;
    default:
      displaytext("Unbekannter Fehler!", 2000);
      return p;
  }

  // OK converted!
  displaytext("Fingerabdruck wird erstellt", 1000);
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    displaytext("Fingerabruck erstellt!", 1000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Kommunikation fehlgeschlagen");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    displaytext("Unterschiedliche Fingerabdrücke!", 5000);
    return p;
  } else {
    displaytext("Unbekannter Fehler!", 2000);
    return p;
  }

  Serial.print("ID 1");
  p = finger.storeModel(1);
  if (p == FINGERPRINT_OK) {
    displaytext("Gespeichert!", 5000);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Kommunikation fehlgeschlagen");
    return p;
  } else {
    displaytext("Unbekannter Fehler!", 2000);
    return p;
  }

  return true;
}
