/*A sketch to get the ESP8266 on the network and connect to a service via HTTP to
   get our external IP address with JSON. This sketch requires <ArduinoJson.h> available
   from Sketch -> Include Library -> Manage Library.
   For readability we have created a new function, getIP() and consolidated/removed some code.
   jeg 2017
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h> //provides the ability to parse and construct JSON objects - great library for parsing

const char* ssid = "CenturyLink0502";
const char* pass = "121212121224G";

void setup() {
  Serial.begin(115200);
  delay(10);
  
  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));

  Serial.print("Connecting to "); Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(""); Serial.println("WiFi connected");
  Serial.print("IP address: "); Serial.println(WiFi.localIP());

  Serial.println(getIP());
}


void loop() {
  //if we put getIP() here, it would ping the endpoint over and over . . . DOS attack?
}

String getIP() {
  HTTPClient theClient;
  String ipAddress;

  //Notice we use http here, vs. https . . . this connection is non-encrypted
  theClient.begin("http://api.ipify.org/?format=json"); //return IP as .json object
  int httpCode = theClient.GET();

  if (httpCode > 0) {
    if (httpCode == 200) {

      //create a buffer to hold our JSON object that is 200 char in length - an object from the Arduino JSON library
      //one you leave the if loop, the JSON library throws away the buffer, so MEMSET is properly handled
      //DON'T EVER PUT A STATIC JSON BUFFER INTO THE main loop();
      StaticJsonBuffer<200> jsonBuffer;

      String payload = theClient.getString(); 
      // root is the paylod that is getting parsed
      // JsonObject& is the pointer to an object in memory called root
      // all the parsing happens inside the buffer
      // root now refers to the results of the parsed buffer
      // all the parsing stays in the buffer and all you do is reference the buffer and fiddle with it
      // and when you use the root label you get the contents of the parsed buffer
      Serial.print("This is the payload: ");
      Serial.println(payload);
      JsonObject& root = jsonBuffer.parse(payload); //parse the payload into the object named 'root'
       
      //JSON objects are comprise of name:value pairs and we can refer to value by name
      //Here, we pull the value associated with the name 'ip' (which is in 'root' and place 
      //it into the String 'ipAddress'
       
      ipAddress = root["ip"].as<String>();   //we must cast the value as a String,
                                             //as the object returns char arrays
    } else {
      Serial.println("Something went wrong hitting the endpoint, check your url?"); //else, print an error statement
      return "error";
    }
  }
  return ipAddress;
}


/*Another parsing option*/

/*
  String payload = theClient.getString();
  char temp[payload.length() + 1];
  payload.toCharArray(temp, payload.length() + 1);
  JsonObject& root = jsonBuffer.parse(payload);
  ipAddress = root["ip"].as<String>();
*/
