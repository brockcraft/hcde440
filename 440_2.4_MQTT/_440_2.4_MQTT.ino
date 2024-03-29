/*ICE #2
   Using a Web Sockets Server. This sketch connects to the public Spacebrew server and
   desmonstrates publishing a button state and message

   jeg 2017
   brc 2018
*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsClient.h>
#include <Spacebrew.h>

//const char* ssid     = "hitlab";
//const char* password = "";
const char* ssid     = "CenturyLink0502";
const char* password = "121212121224G";

char host[] = "sandbox.spacebrew.cc";
char clientName[] = "Brock's ESP8266";
char description[] = "Brock's Huzzah on the network!";

#define buttonPin 2    // the number of the pushbutton pin
#define ledPin 0      // the number of the LED pin

int buttonState = 0;         // variable for holding the pushbutton status
boolean buttonFlag = false;  // variable to check for change in button status
// to only send data to SpaceBrew if button changes

ESP8266WiFiMulti wifi;
Spacebrew sb;

// Declare handlers for various Spacebrew events
// These get defined below!
void onOpen();
void onClose();
void onError(char* message);
void onBooleanMessage(char *name, bool value);
void onStringMessage(char *name, char* message);
void onRangeMessage(char *name, int value);

void setup() {
  Serial.begin(115200);

  //functions to set up the pin behaviours
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // Connect to wifi network
  Serial.println("");
  wifi.addAP(ssid, password);
  while (wifi.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Bind Spacebrew connection event handlers
  sb.onOpen(onOpen);
  sb.onClose(onClose);
  sb.onError(onError);

  // Bind Spacebrew message event handlers
  sb.onBooleanMessage(onBooleanMessage);
  sb.onStringMessage(onStringMessage);
  sb.onRangeMessage(onRangeMessage);

  // Register publishers and subscribers
  // these entities will appear in the Spacebrew server console
  sb.addPublish("Button", SB_BOOLEAN);
  sb.addSubscribe("LED", SB_BOOLEAN);
  sb.addPublish("Message", SB_STRING);
  sb.addSubscribe("Message", SB_STRING);

  // Connect to the spacebrew server
  sb.connect(host, clientName, description);
}

///////////////////////////////CALLBACKS////////////////////////////////////
void loop() {
  sb.monitor();
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed, if it is, the buttonState is LOW:
  if (buttonState == LOW && buttonFlag == false) {
    sb.send("Button", true);
    buttonFlag = true;
  }
  if (buttonState == HIGH && buttonFlag == true) {
    sb.send("Button", false);
    buttonFlag = false;
  }

  if (Serial.available()) {
    String temp;
    if (Serial.available() > 0) {
      temp = Serial.readStringUntil('\n');
    }
    char* message = new char[temp.length() + 1];
    temp.toCharArray(message, temp.length() + 1);
    Serial.print("Sending: ");
    Serial.println(message);
    sb.send("Message", message);
  }
}

///////////////////////////////CALLBACKS////////////////////////////////////
void onBooleanMessage(char *name, bool value) {
  Serial.print("Receiving <<<< ");
  Serial.print("bool: ");
  Serial.print(name);
  Serial.print(" :: ");
  Serial.println(value);
  if (value == true) {
    digitalWrite(ledPin, HIGH);
  }
  if (value == false) {
    digitalWrite(ledPin, LOW);
  }
}

void onStringMessage(char *name, char* message) {
  Serial.print("Receiving <<<< ");
  Serial.print("message: ");
  Serial.println(message);
}

void onRangeMessage(char *name, int value) {

}

void onOpen() {
  //send a message when we get connected!
  sb.send("Message", "Hello Spacebrew");
}

void onClose() {
  Serial.println("Disconnecting from server. Goodbye!");
}

void onError(char* message) {
  Serial.print("Error from server:  ");
  Serial.println(message);
}
