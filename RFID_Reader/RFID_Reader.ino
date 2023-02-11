String rfidpsswd = "";
String temp;

#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(0,2);

void setup() {
  Serial.begin(115200);
  SPI.begin();
  mfrc522.PCD_Init();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    temp.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    temp.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  temp.toUpperCase();
  rfidpsswd = temp;
  temp = "";
  Serial.println(rfidpsswd);
  delay(400);
}