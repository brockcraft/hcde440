//////////
// This program allows us to 'talk' to an esp8266 via an MQTT Server.
// The MQTT messages are parsed to control a servo motor 
// This program only subscribes to the MQTT server, reading messages
// originating at the server
//////////

#include <ESP8266WiFi.h>    //Requisite Libraries . . .
#include "Wire.h"           //
#include <PubSubClient.h>   //
#include <ArduinoJson.h>    //
#include <Servo.h>          //For driving our servo

#define wifi_ssid "hitlab"   //You have seen this before
#define wifi_password "" //
//#define wifi_ssid "CenturyLink0502"   //You have seen this before
//#define wifi_password "121212121224G" //

#define mqtt_server "mediatedspaces.net"  //this is its address, unique to the server
#define mqtt_user "hcdeiot"               //this is its server login, unique to the server
#define mqtt_password "esp8266"           //this is it server password, unique to the server

int pos = 90;     //We create a variable to hold the servo position and set it to 90 (degrees)
int oldPos = -1;  //A variable to hold the former position, so we write a new pos the servo pos changes

WiFiClient espClient;             //blah blah blah, espClient
PubSubClient mqtt(espClient);     //blah blah blah, tie PubSub (mqtt) client to WiFi client

Servo myservo;    //And create a servo object, too!

char mac[6]; //A MAC address is a 'truly' unique ID for each device, lets use that as our 'truly' unique user ID!!!

/////SETUP/////
void setup() {
  Serial.begin(115200);
  Serial.println("Started.");
  myservo.attach(2);                   //attach the servo library to pin 2

  setup_wifi();
  mqtt.setServer(mqtt_server, 1883);
  mqtt.setCallback(callback); //register the callback function

}

/////SETUP_WIFI/////
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");  //get the unique MAC address to use as MQTT client ID, a 'truly' unique ID.
  WiFi.macAddress((byte*)mac); // .macAddress returns byte array : 6 bytes representing the MAC address of your shield
  Serial.println(WiFi.macAddress());  //.macAddress returns a byte array 6 bytes representing the MAC address
}                                     //5C:CF:7F:F0:B0:C1 for example


/////CONNECT/RECONNECT/////Monitor the connection to MQTT server, if down, reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (mqtt.connect(mac, mqtt_user, mqtt_password)) { //<<---using MAC as client ID, always unique!!!
      Serial.println("connected");
      mqtt.subscribe("theTopic/+"); //we are subscribing to 'theTopic' and all subtopics below that topic
    } else {                        //please change 'theTopic' to reflect your topic you are subscribing to
      Serial.print("failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

/////LOOP/////
void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop(); //this keeps the mqtt connection 'active'
  if (oldPos != pos) { //if the pos changes, then write the new position to the servo
    myservo.write(pos);
    oldPos = pos; //the current pos become the former pos, and we wait for a new pos to come in from MQTT
  }
}

/////CALLBACK/////

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic); //'topic' refers to the incoming topic name, the 1st argument of the callback function
  Serial.println("] ");
  DynamicJsonBuffer  jsonBuffer; //blah blah blah a DJB
  JsonObject& root = jsonBuffer.parseObject(payload); //parse it!
  if (!root.success()) { //well?
    Serial.println("parseObject() failed, are you sure this message is JSON formatted.");
    return;
  }
  root.printTo(Serial); //print out the parsed message

// if the topic payload has to do with the servo, move the servo  
  if (strcmp(topic, "theTopic/servo") == 0) {
    pos = root["servo"].as<int>(); //read the value from the parsed string and set it to pos
    myservo.write(pos);
  }

  Serial.println(); //give us some space on the serial monitor read out
}
