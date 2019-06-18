#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <String>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <MPU6050_tockn.h>
#include <Wire.h>

//Task for reading sensor data from Arduino Mega
TaskHandle_t megaSerialTask;
void megaSerialTaskcode( void * pvParameters );

// Constants
const char* ssid = "CS Airlines";
//const char* ssid = "Airlines";
const char* password = "09876543";
String flightCommand = "m:0000s0:090s1:090s2:090s3:090s4:090r:0";

const int motorPin[]={13,12,14,27};
const int volanPin[]={19,18,25,33,5}; //s0-wing-left-19 s1-wing-right-18 s2-tail-left-17 s3-tail-right-16 s4-centar-5
const int motorCurrentPin[]={35,34,39,36};
const int batteryVoltagePin=32;

const int noMotors=4;
const int noVolans=5;
const int adcMax=4095;
const int pwmRes=1023;
const float maxADCVoltage=3.3;

const int lbnd[]={1784,1159,1642,1674};//const int lbnd[]={1765,1159,1628,1633};
const int ubnd[]={1880,1666,1737,2220};

const int lvlmt=30;
const int uvlmt=150;

const float maxBatteryVoltage=12.45;
const float minBatteryVoltage=8;
const long batteryVoltageR1=9940;
const long batteryVoltageR2=38200;

const int minMotorCurrent[]={2118,2124,2084,2084};
const int maxMotorCurrent[]={5000,5000,5000,5000};
const int maxCurrentMeter=20;

Servo motor[noMotors];
Servo volan[noVolans];

const int freq = 5000;
const int resolution = 10;


// Globals
int i,refreshBtn = 0;

int volanAngle[] = {90,90,90,90,90};
long throtle, realThrotle, tempThrotle;
int rawMotorCurrentMeter[4],rawBatteryVoltage,batteryPercentage;
float motorCurrent[4],batteryVoltage;

WebSocketsServer webSocket = WebSocketsServer(80);
int pingPeriod = 200;
unsigned long timeLastPing = 0;

MPU6050 mpu6050(Wire); //Gyro

String megaSerialString;

// Called when receiving any WebSocket message
void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  // Figure out the type of WebSocket event
  switch(type) {
    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connection from ", num);
        Serial.println(ip.toString());
      }
      break;

    // Recived message from client
    case WStype_TEXT:
      Serial.printf("[%u] Command: %s\n", num, payload);
      //reinterpret_cast<char*>(payload);
      flightCommand = "" + String(reinterpret_cast<char*>(payload));
      //flightCommand = "m:0000w:000e:000t:000r:0"
      throtle = flightCommand.substring(2,6).toInt();
      volanAngle[0] = flightCommand.substring(8,11).toInt();
      volanAngle[1] = flightCommand.substring(8,11).toInt();
      volanAngle[2] = flightCommand.substring(13,16).toInt();
      volanAngle[3] = 180 - flightCommand.substring(13,16).toInt();
      volanAngle[4] = flightCommand.substring(18,21).toInt();
      refreshBtn = flightCommand.substring(23,24).toInt();
      //webSocket.broadcastTXT(flightCommand.c_str());
      //webSocket.sendTXT(0, flightCommand);
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

void resetMotors(){
  for(i = 0; i < noMotors;i++){
    motor[i].writeMicroseconds(600);
  }
  delay(2000);
}

void setup() {
  // Start Serial ports
  Serial.begin(115200);
  Serial2.begin(9600);

  // start Gyro
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

  //create a task that will be executed in the megaSerialTaskcode() function, with priority 1 and executed on core 1
                      // Task function,name of task,Stack size of task,parameter of the task, priority of the task, Task handle to keep track of created task,pin task to core 
  xTaskCreatePinnedToCore(megaSerialTaskcode,"megaSerialTask",10000,NULL,1,&megaSerialTask,1);

  //Create access point
  WiFi.softAP(ssid, password);

  //OTA updatee handle
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  // Print our IP address
  Serial.println("WiFi UP!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  for(i = 0; i < noMotors; i++){
    motor[i].attach(motorPin[i]);
  }
  resetMotors();
  
  for(i = 0; i < noVolans; i++){
    volan[i].attach(volanPin[i]);
  }
  
  pinMode(batteryVoltagePin,INPUT);
  
  for(i = 0; i < noMotors; i++){
    pinMode(motorCurrentPin[i],INPUT);
  }
  
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
 
 delay(2000);
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
      megaSerialString = Serial2.readStringUntil('!');	  
	    megaSerialString = "[M] Sensors: " + megaSerialString;
      if(Serial.availableForWrite()){
        Serial.println(megaSerialString);
      }
    }
	delay(200);
  }
}

void loop() {
    //OTA update handle
  ArduinoOTA.handle();
  
  // Look to handle WebSocket data
  webSocket.loop();

  if(refreshBtn)
    resetMotors();

  tempThrotle=throtle;
  
  for(i = 0; i < noMotors;i++){
    realThrotle=map(tempThrotle,0,100,lbnd[i],ubnd[i]);
    if(tempThrotle==0)
      motor[i].writeMicroseconds(lbnd[i]-100);
    else
      motor[i].writeMicroseconds(realThrotle);
  }
  //motor[0].writeMicroseconds(throtle);

  rawBatteryVoltage=analogRead(batteryVoltagePin);
  float Vin=map(rawBatteryVoltage,0,adcMax,0.0,maxADCVoltage);
  batteryVoltage=Vin*(batteryVoltageR1+batteryVoltageR2)/batteryVoltageR1;
  batteryPercentage=map(batteryVoltage,minBatteryVoltage,maxBatteryVoltage,1,100);

  for(i = 0; i < noMotors;i++){
    rawMotorCurrentMeter[i]=analogRead(motorCurrentPin[i]);
    motorCurrent[i]=map(rawMotorCurrentMeter[i],minMotorCurrent[i],adcMax,0.0,maxCurrentMeter);
  }
  
  for(i=0;i<noVolans;i++){
    volan[i].write(constrain(map(volanAngle[i],0,180,lvlmt,uvlmt),lvlmt,uvlmt));
  }

  //get Gyro reading
  mpu6050.update();

  //ping connected clients
  if(millis() > timeLastPing + pingPeriod){
        timeLastPing = millis();
        webSocket.broadcastTXT(megaSerialString + "Angle X: " + String(mpu6050.getAngleX()) + "Angle Y: " + String(mpu6050.getAngleY()));
        //"+ rawBatteryVoltage: " + String(rawBatteryVoltage) + " batteryVoltage: " + String(batteryVoltage) + " bateryPercantage: " + String(batteryPercentage) +" Vin " + String(Vin)) + 
    }
}