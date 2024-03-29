#include "Wire.h"
//#include <LiquidTWI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#define wifi_ssid ""
#define wifi_password ""
#define mqtt_server "mediatedspaces.net"
#define mqtt_user "hcdeiot" //your user name should be unique
#define mqtt_password "esp8266" //still using login/pass authorization for MQTT server
#define topic_name "brock/allthedata" //topic you are subscribing to

//LiquidTWI lcd(0);
WiFiClient espClient;
PubSubClient client(espClient);

//some arrays to hold our parsed data
const char* temp;
const char* humidity;
const char* message;

char incoming[200]; //an array to hold the incoming message

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
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

//here is our callback
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    incoming[i] = (char)payload[i];  //payload is an array of bytes (characters) that we put into incoming[]
  }
  Serial.println(incoming); //just to debug, is it there?

  DynamicJsonBuffer  jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(incoming);

  if (!root.success()) {
    Serial.println("parseObject() failed");
    return;
  }

  temp = root["temp"].asString();
  humidity = root["humidity"].asString();
  message = root["message"].asString();

  char line1[20]; //some arrays to hold our OLED lines
  char line2[20];
  char line3[20];

// might just be able to write these direct to the oled

  
//  sprintf(line1, " Temp is %sF ", temp); //we use sprintf to format our lines (look up sprintf)
//  sprintf(line2, "Humidity  %s%", humidity);
//  sprintf(line3, "Message  %s%", message);
//
// Serial.println(line1);
// Serial.println(line2);
// Serial.println(line3);
  
//  lcd.clear();
//  lcd.home();
//  lcd.print(line1);
//  lcd.setCursor(0, 1);
//  lcd.print(line2);
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("A unique name", mqtt_user, mqtt_password)) { //<-- a unique name, please.
      Serial.println("connected");
      //client.subscribe("JasonG");
      client.subscribe("brock/allthedata");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
//  lcd.begin(16, 2);

  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
// next line can be used for testing a connection
//  client.publish(topic_name, "testing");
  delay(2000);
}
