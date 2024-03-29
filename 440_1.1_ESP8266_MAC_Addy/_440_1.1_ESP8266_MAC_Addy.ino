//Find the MAC address and other stuff regarding your esp8266

#include "ESP8266WiFi.h"

String s;

void setup() {
  pinMode(0, OUTPUT);
  Serial.begin(115200);


  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
  s = WiFi.macAddress();
  Serial.println("Your mac address is: " + s);

  s = WiFi.softAPmacAddress();
  Serial.println("Your soft mac address is: " + s);

  s = ESP.getFreeSketchSpace();
  Serial.println("Your free sketch space is: " + s);

  s = ESP.getSketchSize();
  Serial.println("Your sketch size is: " + s);

  s = ESP.getFlashChipRealSize();
  Serial.println("Your chip size is: " + s);

  s = ESP.getSdkVersion();
  Serial.println("Your SDK version is: " + s);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(0, HIGH);
  delay(500);
  digitalWrite(0, LOW);
  delay(500);

}

