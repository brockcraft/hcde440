//////////
//Super commenting is ON . . .
//This program allows us to 'talk' to an esp8266 via the NODE-Red dashboard.
//The NR flow subscribes to the MQTT server and then publishes messages via
//the Color Picker, Switch, Form, and Slide modules, providing a way to change
//the state of an RGB led, an OLED, and servo on the remote device. This program
//only subscribes to the MQTT server, reading messages originating at the server
//via the dashboard (web) interface, but there is no reason that one could
//not publish from the remove device as well.
//////////

#include <ESP8266WiFi.h>    //For WiFi connectivity
#include "Wire.h"           //For enabling i2c
#include <PubSubClient.h>   //For traversing the MQTT server
#include <ArduinoJson.h>    //For parsing/reading our JSON strings
#include <Servo.h>          //For driving our servo
#include <gfxfont.h>        //For the OLED display
#include <Adafruit_GFX.h>   //For the OLED display
#include <Adafruit_SSD1306.h>  //For the OLED display

#define wifi_ssid "University of Washington"   //You have seen this before
#define wifi_password "" //
//#define wifi_ssid "CenturyLink0502"   //You have seen this before
//#define wifi_password "121212121224G" //
//////////
//So to clarify, we are connecting to and MQTT server
//that has a login and password authentication
//////////

#define mqtt_server "mediatedspaces.net"  //this is its address, unique to the server
#define mqtt_user "hcdeiot"               //this is its server login, unique to the server
#define mqtt_password "esp8266"           //this is it server password, unique to the server

//////////
//We also need to publish and subscribe to topics, for this sketch are going
//to adopt a topic/subtopic addressing scheme: topic/subtopic
//////////

WiFiClient espClient;             //blah blah blah, espClient
PubSubClient mqtt(espClient);     //blah blah blah, tie PubSub (mqtt) client to WiFi client

#define theTopic "theTopic/+"//we are subscribing to 'theTopic' and all subtopics below that topic
//please change 'theTopic' to reflect your topic you are subscribing to

//////////
//We need a 'truly' unique client ID for our esp8266, all client names on the server must be unique.
//Every device, app, other MQTT server, etc that connects to an MQTT server must have a unique client ID.
//This is the only way the server can keep every device separate and deal with them as individual devices/apps.
//The client ID is unique to the device.
//////////

char mac[6]; //A MAC address is a 'truly' unique ID for each device, lets use that as our 'truly' unique user ID!!!

//////////
//In our loop(), we are going to create a c-string that will be our message to the MQTT server, we will
//be generous and give ourselves 200 characters in our array, if we need more, just change this number
//////////

char message[201]; //201, as last character in the array is the NULL character, denoting the end of the array

//////////
//Initialize pins to drive our 3-color led, which we use in this example.
//You could use 3 differing color leds to achive the same effect
//////////

const int redPin = 12;
const int greenPin = 14;
const int bluePin = 13;

//And here are three variables (unsigned ints) to hold our 3 color values
//we use unsigned ints to prevent the possibility of negative (-) values
unsigned int r; //red
unsigned int g; //green
unsigned int b; //blue

//This global variable holds the state of the switch/toggle, if true, the light
//will be on, if false, the light will be off.
boolean toggle = true;

int pos = 90;     //We create a variable to hold the servo position and set it to 90 (degrees)
int oldPos = -1;  //A variable to hold the former position, so we write a new pos the servo pos changes


//objects

// create OLED Display instance on an ESP8266
// set OLED_RESET to pin 15 (or another), because we are using default I2C pins D4/D5.
#define OLED_RESET 15
Adafruit_SSD1306 display(OLED_RESET);

Servo myservo;    //And create a servo object, too!

/////SETUP/////
void setup() {

  Serial.begin(115200); //we keep the Serial lines fast as there is a good deal of activity to catch
  Serial.println("_440_4.2_TricolorLED_Subscribe Started.");
  Serial.println("");
  setup_wifi();         //setup the wifi

  mqtt.setServer(mqtt_server, 1883);  //set the server for the PubSub client
  mqtt.setCallback(callback);         //register the callback function

  pinMode(redPin, OUTPUT);            //set our led pins to OUTPUT mode, they will send out data
  pinMode(greenPin, OUTPUT);          //which in this case is a voltage (pwm)
  pinMode(bluePin, OUTPUT);           //

  myservo.attach(2);                   //attach the servo library to pin 2

  // set up the OLED display
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)

  Adafruit_SSD1306 display(OLED_RESET); //start up the OLED
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Starting up.");
  display.display();
  rgbSet(0,255,0);
}


/////SETUP_WIFI/////
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);              // We start by connecting to a WiFi network
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) { //if not connected, try again . . . forever
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected.");  //get the unique MAC address to use as MQTT client ID, a 'truly' unique ID.
  WiFi.macAddress((byte*)mac); // .macAddress returns byte array : 6 bytes representing the MAC address of your shield
  Serial.println(WiFi.macAddress());  //
}


/////CONNECT/RECONNECT/////Monitor the connection to MQTT server, if down, reconnect
void reconnect() {
  // Loop until we're reconnected
  while (!mqtt.connected()) {
    Serial.print("Attempting MQTT connection...");
    Serial.print("The MQTT ID is: ");
    Serial.println(mac);
    if (mqtt.connect(mac, mqtt_user, mqtt_password)) { //<<---using MAC as client ID, always unique!!!
      Serial.println("connected");
      mqtt.subscribe(theTopic); //we are subscribing to 'theTopic' and all subtopics below that topic
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
//Not much here, the majority of the activity occurs in the callback, which
//only happens when a message is received.
//////////

void loop() {
  if (!mqtt.connected()) {
    reconnect();
  }
  mqtt.loop();

  //We are dealing with the servo in the loop because the servo needs to be continously updated
  //in order to complete its positioning.
  if (oldPos != pos) { //if the pos changes, then write the new position to the servo
    myservo.write(pos);
    oldPos = pos; //the current pos become the former pos, and we wait for a new pos to come in from MQTT
  }
}


/////RGB_SET/////
//Sets the rgb colors for the tri-color led
/////
void rgbSet(int r, int g, int b) { //this function takes 3 arguments
  analogWrite(redPin, r);   //write red value
  analogWrite(greenPin, g); //write green value
  analogWrite(bluePin, b);  //write blue value
}

/////CALLBACK/////
//The callback is where we attacch a listener to the incoming messages from the server.
//By subscribing to a specific channel or topic, we can listen to those topics we wish to hear.
//We place the callback in a separate tab so we can edit it easier . . . (will not appear in separate
//tab on github!)
/////

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.println();
  Serial.print("Message arrived [");
  Serial.print(topic); //'topic' refers to the incoming topic name, the 1st argument of the callback function
  Serial.println("] ");

  /////
  //In this example, everything gets parsed that arrives via the subscription
  //if it does not correspond to one of the topics below, it is simply ignored . . .
  /////

  DynamicJsonBuffer  jsonBuffer; //blah blah blah a DJB
  JsonObject& root = jsonBuffer.parseObject(payload); //parse it!

  if (!root.success()) { //well?
    Serial.println("parseObject() failed, are you sure this message is JSON formatted.");
    return;
  }

  /////
  //We can use strcmp() -- string compare -- to check the incoming topic in case we need to do something
  //special based upon the incoming topic, like move a servo or turn on a light . . .
  //strcmp(firstString, secondString) == 0 <-- '0' means NO differences, they are ==
  /////

  //if subtopic is 'led'
  if (strcmp(topic, "theTopic/led") == 0) {
    Serial.println("A message for the RGB led . . ."); //debug message

    r = root["r"].as<int>();  //read the r, g, b values from the parsed JSON string
    g = root["g"].as<int>();
    b = root["b"].as<int>();

    rgbSet(r, g, b);          //and set those values to the leds, via the rgbSet() function

    if (toggle == false) {    //if the toggle is off (false) set the colors to (0, 0, 0), or off
      rgbSet(0, 0, 0);
    }
  }

  //if subtopic is 'light'
  else if (strcmp(topic, "theTopic/light") == 0) {
    Serial.println("As message for the light, on/off . . .");

    toggle = root["light"].as<boolean>(); //read the value out as boolean

    if (toggle == true) { //if true, set to current r, g, b values
      Serial.print("light on");
      rgbSet(r, g, b);
    }

    else if (toggle == false) { //otherwise, set the colors to zero, OFF
      Serial.print("light off");
      rgbSet(0, 0, 0);
    }
  }

  //if subtopic is 'message'
  else if (strcmp(topic, "theTopic/message") == 0) {
    Serial.println("A message is being delivered . . .");

    String line1 = root["line1"].as<String>();  //read line1 values and put them in a String
    String line2 = root["line2"].as<String>();  //read line2 values . . .

    display.clearDisplay();            //clear the OLED screen
    display.setCursor(0, 0);    //go to column 0, line 0 . . . the upper left hand corner of the OLED
    display.println(line1);       //print line1 to the screen
    display.println(line2);       //print line2 to the screen
    display.display();
  }

  //if the subtopic is servo
  else if (strcmp(topic, "theTopic/servo") == 0) {
    pos = root["servo"].as<int>(); //read the value from the parsed string and set it to pos
  }
  root.printTo(Serial); //print out the parsed message to the Serial port for debugging
  myservo.write(pos);
  Serial.println(); //give us some space on the serial monitor read out
}
