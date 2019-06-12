#include <Arduino.h>

//Task for reading sensor data from Arduino Mega
TaskHandle_t megaSerialTask;
void megaSerialTaskcode( void * pvParameters );

String megaSerialString;

void setup() {
  // put your setup code here, to run once:
  Serial2.begin(115200);
  Serial.begin(115200);

  xTaskCreatePinnedToCore(megaSerialTaskcode,"megaSerialTask",10000,NULL,1,&megaSerialTask,1);
    delay(500); 
}

//megaSerialTaskcode: blinks an LED every 700 ms
void megaSerialTaskcode( void * pvParameters ){
  if(Serial.availableForWrite()){
    Serial.print("megaSerialTask running on core ");
    Serial.println(xPortGetCoreID());
  }

  for(;;){
    // put your main code here, to run repeatedly:
    if(Serial2.available()){
      megaSerialString = Serial2.readString();
      Serial.println("[1] : Read finished");
    }
  }
}

void loop() {
  Serial.print("[0] : " + megaSerialString);
  delay(200);
}