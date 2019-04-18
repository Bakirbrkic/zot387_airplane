#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <esp_wifi.h>

//const
String ssid = "Airlines";
String pass = "09876543";

void setup() {
  Serial.begin( 115200 );

  WiFi.begin();
  delay( 500 );
  ESP_ERROR_CHECK( esp_wifi_set_protocol( WIFI_IF_STA, WIFI_PROTOCOL_LR ) );
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid.c_str(), pass.c_str());
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  Serial.println(WiFi.localIP());
}

void loop() {
  
  delay(1000);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("connected");
  } else {
    Serial.println("Connection Lost!!!");    
  }
  

}
