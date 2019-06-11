#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <String>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

//Task for reading sensor data from Arduino Mega
TaskHandle_t megaSerialTask;
void megaSerialTaskcode( void * pvParameters );


// Constants
const char* ssid = "CS Airlines - ZoT387";
const char* password = "09876543";
String flightCommand = "m:0000s0:090s1:090s2:090s3:090s4:090r:0";

const int motorPin[]={13,12,14,27};
const int volanPin[]={19,18,17,16,5}; //s0-wing-left-19 s1-wing-right-18 s2-tail-left-17 s3-tail-right-16 s4-centar-5

const int noMotors=4;
const int noVolans=5;
const int adcMax=4095;
const int pwmRes=1023;

const int lbnd[]={1742,1160,1619,1615};
const int ubnd[]={1850,1670,1730,2160};

Servo motor[noMotors];
Servo volan[noVolans];

const int freq = 5000;
const int resolution = 10;


// Globals
int i,refreshBtn = 0;

int volanAngle[] = {90,90,90,90,90};
long throtle, realThrotle, tempThrotle;

WebSocketsServer webSocket = WebSocketsServer(80);
int pingPeriod = 200;
unsigned long timeLastPing = 0;

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
      //flightCommand = "m:0000s0:090s1:090s2:044s3:090s4:090r:0"
      throtle = flightCommand.substring(2,6).toInt();
      volanAngle[0] = flightCommand.substring(9,12).toInt();
      volanAngle[1] = flightCommand.substring(15,18).toInt();
      volanAngle[2] = flightCommand.substring(21,24).toInt();
      volanAngle[3] = flightCommand.substring(27,30).toInt();
      volanAngle[4] = flightCommand.substring(33,36).toInt();
      refreshBtn = flightCommand.substring(38,39).toInt();
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
  delay(3000);
}



void setup() {
  // Start Serial port
  Serial.begin(115200);

  //create a task that will be executed in the megaSerialTaskcode() function, with priority 1 and executed on core 1
                             // Task function,name of task,Stack size of task,parameter of the task, priority of the task, Task handle to keep track of created task,pin task to core 
  xTaskCreatePinnedToCore(megaSerialTaskcode,"megaSerialTask",10000,NULL,1,&megaSerialTask,1);
    delay(500); 

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
  for(i = 0; i < noMotors; i++){
    motor[i].writeMicroseconds(lbnd[i]);
  }
  for(i = 0; i < noVolans; i++){
    volan[i].attach(volanPin[i]);
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
    if(Serial.availableForWrite())//check if the resource is available to avoid deadlock and kernel panic on cores
      Serial.println("CORE 2 ON");
    delay(700);
    if(Serial.availableForWrite())
      Serial.println("CORE 2 OFF");
    delay(700);
  }
}

void loop() {
    //OTA update handle
  ArduinoOTA.handle();
  
  // Look for and handle WebSocket data
  webSocket.loop();
  if(refreshBtn)
    resetMotors();
  tempThrotle=throtle;
  for(i = 0; i < noMotors;i++){
    realThrotle=map(tempThrotle,0,adcMax,lbnd[i],ubnd[i]);
    if(tempThrotle==0)
      motor[i].writeMicroseconds(lbnd[i]-100);
    else 
      motor[i].writeMicroseconds(realThrotle);
  }
  
  for(i=0;i<noVolans;i++){
    volan[i].write(volanAngle[i]);
  }
  //ping connected clients
  if(millis() > timeLastPing + pingPeriod){
        timeLastPing = millis();
        //webSocket.connectedClients(true);
        webSocket.broadcastTXT("c " + flightCommand);
    }
}