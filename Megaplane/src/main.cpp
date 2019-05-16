#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

int getDist(const int, const int);
void namaSerial();
void espuSerial();

const int trigPin = 35;
const int echoPin = 34;

void setup() {
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	Serial1.begin(115200);
	Serial.begin(9600);
}

int dist;
double temp,pres,alt;
Adafruit_BMP280 tandp;

void loop() {
	dist=getDist(trigPin,echoPin);
	temp=tandp.readTemperature();
	pres=tandp.readPressure();
	alt=tandp.readAltitude(1013.25);
	namaSerial();
	espuSerial();
	delay(200);
}

int getDist(const int tp, const int ep){
	long duration;
	digitalWrite(tp, LOW);
	delayMicroseconds(2);
	digitalWrite(tp, HIGH);
	delayMicroseconds(10);
	digitalWrite(tp, LOW);
	duration = pulseIn(ep, HIGH);
	return duration*0.034/2;
}

void namaSerial(){
	Serial.print("Distance = ");
	Serial.println(dist);

	Serial.print("Temperature = ");
    Serial.print(temp);
    Serial.println(" *C");
    
    Serial.print("Pressure = ");
    Serial.print(pres);
    Serial.println(" Pa");

    Serial.print("Approx altitude = ");
    Serial.print(alt); // this should be adjusted to your local forcase
    Serial.println(" m");
}

void espuSerial(){
	Serial1.print("d:");
	Serial1.print(dist);
}