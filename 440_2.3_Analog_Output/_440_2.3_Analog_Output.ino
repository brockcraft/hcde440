/*ICE #2
   Analog signal via HUZZAH ADC

   brc 2018
*/

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(115200);
  pinMode(2, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(2) == LOW) {
    int analogValue = analogRead(0);
    Serial.println(analogValue);
  }
  delay(250);
}

