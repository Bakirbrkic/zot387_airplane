#include <ESP32_Servo.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <String>

// Constants
const char* ssid = "Himzo i Lejla";
const char* password = "naspratu";
const int ledPin = 12;
Servo ser;

String flightCommand = "from the other side: ";

int ledState = 0;
int servoState = 0;

// Globals
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
      Serial.printf("[%u] Text: %s\n", num, payload);
      //reinterpret_cast<char*>(payload);
      flightCommand = "" + String(reinterpret_cast<char*>(payload));
      ledState = flightCommand.substring(4,5).toInt();
      servoState  = flightCommand.substring(9).toInt();
      
      //digitalWrite(ledPin, ledState);
      
      Serial.printf("LedState: %d\nServoState: %d\n", ledState, servoState);
      webSocket.sendTXT(num, flightCommand.c_str());
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

void setup() {
  // Start Serial port
  Serial.begin(115200);

  //LED
  pinMode(ledPin, OUTPUT);
  //servo
  ser.attach(14);

  // Connect to access point
  Serial.println("Connecting");
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
  }

  // Print our IP address
  Serial.println("Connected!");
  Serial.print("My IP address: ");
  Serial.println(WiFi.localIP());

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);
}

void loop() {

  // Look for and handle WebSocket data
  webSocket.loop();

  digitalWrite(ledPin, ledState);
  ser.write(servoState);

  delay(10);
}
