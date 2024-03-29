/*A sketch to get the ESP8266 on the network and report the local IP address for the unit.
 * jeg 2017
 */

#include <ESP8266WiFi.h> //affords WiFi funtionality

const char* ssid = "University of Washington"; //service set identifier (network name)
const char* pass = ""; //network password - for UW use no password, verification is by MAC address at UW

// you could set this up to try multiple ssids for using this at home

void setup() {
  Serial.begin(115200); //for debugging
  delay(10);
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.print("Connecting to ");
  
  Serial.println(ssid);
  
  WiFi.mode(WIFI_STA); //WiFi_STAtion Mode (vs. WiFi_AP, access point mode)
  WiFi.begin(ssid, pass); //method to start WiFi and make connection to service

  while (WiFi.status() != WL_CONNECTED) { //while ! connected
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); //method returns localIP address (LAN)
}

void loop() {
//Loops forever
}
