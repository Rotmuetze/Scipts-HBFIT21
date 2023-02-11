#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "HUSKYLENS.h"
HUSKYLENS huskylens;


#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

#define OLED_RESET -1        // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Gleichstrommotor links

int GSM1 = D8;
int in1 = D3;
int in2 = D4;

// Gleichstrommotor rechts

int in3 = D7;
int in4 = D6;
int GSM2 = D5;

int maxspeed = 150;
String command = "";
boolean drive = false;
void setup() {

  Serial.begin(115200);



  Wire.begin();

  while (!huskylens.begin(Wire))  //Gibt es eine hergestellte I2C Verbindung?
  {
    huskylens.begin(Wire);
    Serial.println("IC2 Verbindung fehlgeschlagen...");
  }
  Serial.println("");
  Serial.println("I2C Verbindung erfolgreich hergestellt.");
  Serial.println("");
  huskylens.writeAlgorithm(ALGORITHM_LINE_TRACKING);
  delay(100);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000);  // Pause for 1 second

  // Clear the buffer
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.clearDisplay();


  pinMode(GSM1, OUTPUT);
  pinMode(GSM2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {

  if (huskylens.request()) {      //Besteht eine I2C Verbindung mit der Huskylens?
    if (huskylens.isLearned()) {  //Hat die Huskylens irgendwas gelernt / sich schon was gemerkt?
      if (huskylens.available()) {
        while (huskylens.available() > 0) {  //Solange die Huskylens mehr als 0 Objekte vor sich erkennt...
          HUSKYLENSResult result = huskylens.read();
          if (result.xTarget <= 179 && result.xTarget >= 141) {
          if (!drive) {
          Serial.println("anfahren");
          anfahren(200);
          command = "anfahren";

          }
          digitalWrite(in1, 0);
          digitalWrite(in2, 1);
          digitalWrite(in3, 0);
          digitalWrite(in4, 1);

          analogWrite(GSM1, 200);
          analogWrite(GSM2, 200);
          delay(10);
          command = "fahren";
          Serial.println("fahren");
          }
       
          else if (result.xTarget <=130 && result.xTarget >= 0) {
            Serial.println("links");
            command = "links";
            rechts();
          
          }

         else if (result.xTarget >= 190) {
           Serial.println("rechts");
            command = "rechts";
          links();
            
          }
          huskylens.customText(command, 20, 20);
        }
      }}
    }
  }



void anfahren(int max) {
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  analogWrite(GSM1, 0);

  digitalWrite(in3, 0);
  digitalWrite(in4, 1);
  analogWrite(GSM2, 0);

  for (int i = 50; i <= max; i = i + 10) {
    analogWrite(GSM1, i);
    analogWrite(GSM2, i);
    delay(100);


    Serial.println(i);
  }
  drive = true;
}




void bremsen(int von, int bis) {

  for (int i = von; i >= bis; i = i - 10) {
    analogWrite(GSM1, i);
    analogWrite(GSM2, i);
    delay(100);
    Serial.println(i);
  }
  if (bis == 0) {
  drive = false;
  }
}

void links(){
  digitalWrite(in1, 0);
  digitalWrite(in2, 1);
  
  analogWrite(GSM1, 150);

  digitalWrite(in3, 0);
  digitalWrite(in4, 0);

  analogWrite(GSM2, 200);
  
  delay(5);
}

void rechts(){
  digitalWrite(in1, 0);
  digitalWrite(in2, 0);
  
  analogWrite(GSM1, 150);

  digitalWrite(in3, 0);
  digitalWrite(in4, 1);

  analogWrite(GSM2, 150);
  
  delay(5);
}


  