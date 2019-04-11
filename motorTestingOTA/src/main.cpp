#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <String>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

// Constants
const char* ssid = "Airlines";
const char* password = "09876543";
String flightCommand = "m:0000s1:000s2:000s3:000s4:000";

const int motorPin[]={33,32,23,19};

const int noMotors=4;
const int adcMax=4095;

const int lbnd[]={1735,1605,1615,1615};
const int ubnd[]={1850,2150,1710,2150};

Servo motor[noMotors];


// Globals
int i,refreshBtn;
long throtle, realThrotle, tempThrotle;
WebSocketsServer webSocket = WebSocketsServer(80);

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

    // Echo text message back to client
    case WStype_TEXT:
      //Serial.printf("[%u] Text: %s\n", num, payload);
      //reinterpret_cast<char*>(payload);
      flightCommand = "" + String(reinterpret_cast<char*>(payload));
        //flightCommand = "m:0000s1:000s2:000s3:000s4:000s5:000r:0"
      throtle = flightCommand.substring(2,6).toInt();
      refreshBtn = flightCommand.substring(38,39).toInt();
      webSocket.broadcastTXT(flightCommand.c_str());
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
  for(i=0;i<noMotors;i++){
    motor[i].writeMicroseconds(600);
  }
  delay(3000);
}

void setup() {
  // Start Serial port
  Serial.begin(115200);

  // Connect to access point
  /*
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }
  */

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
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  for(i=0;i<noMotors;i++){
    motor[i].attach(motorPin[i]);
  }
  resetMotors();
  for(i=0;i<noMotors;i++){
    motor[i].writeMicroseconds(lbnd[i]);
  }
  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
  
  delay(2000);
}

void loop() {
  //OTA update handle
  ArduinoOTA.handle();
  
  // Look for and handle WebSocket data
  webSocket.loop();
  if(refreshBtn)
    resetMotors();
  tempThrotle=throtle;
  for(i=0;i<noMotors;i++){
    realThrotle=map(tempThrotle,0,adcMax,lbnd[i],ubnd[i]);
    motor[i].writeMicroseconds(realThrotle);
  }
}
