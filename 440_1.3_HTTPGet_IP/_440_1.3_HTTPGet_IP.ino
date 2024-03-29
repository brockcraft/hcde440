/*A sketch to get the ESP8266 on the network and connect to a service via HTTP to
   get the external IP address.
   jeg 2017
   
   You can use this to, for example, get an external IP address for using with APIs such as weather.com
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h> //affords HTTP functionality to your sketch

HTTPClient theClient; //instantiate a new HTTPClient object

const char* ssid = "CenturyLink0502";
const char* pass = "121212121224G";

void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.print("This board is running: ");
  Serial.println(F(__FILE__));
  Serial.print("Compiled: ");
  Serial.println(F(__DATE__ " " __TIME__));
  Serial.print("Connecting to ");

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  theClient.begin("http://api.ipify.org/"); //attempt client connection to endpoint/url
  
  int httpCode = theClient.GET(); //a method to execute a GET request and return status code

  if (httpCode > 0) { //if httpCode is -1, the endpoint was not reached
    
    if (httpCode == 200) { //if httpCode is 200, then success!!!
      
      //create a String object (payload) to hold the incoming string
      //read the string with the .getString() method
      String payload = theClient.getString(); 

      Serial.println("The external IP address is: " + payload);//print the result to Serial
      
    } else {
      Serial.println("Something went wrong"); //else, print an error statement
    }
  }
}

void loop() {
}
