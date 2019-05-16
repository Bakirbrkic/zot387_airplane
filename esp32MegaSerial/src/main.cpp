#include <Arduino.h>

String dist;

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial2.available()){
    //int inByte = Serial2.read();
    //Serial.write(inByte);
    //Serial.println(inByte);
    dist = Serial2.readString();
    //dist = dist.substring(8);
    Serial.println(dist);
  }
}