#include <Arduino.h>
#include <BME280I2C.h>
#include <Wire.h>

int getDist(const int, const int);
void namaSerial();
void espuSerial();

const int trigPin = 35;
const int echoPin = 34;

BME280I2C tandp;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

void setup() {
	pinMode(trigPin, OUTPUT);
	pinMode(echoPin, INPUT);
	Serial1.begin(115200);
	Serial.begin(9600);
	if(!tandp.begin()) {
		Serial.println("Could not find a valid BME280 sensor, check wiring!");
		while (1);
	}
	switch(tandp.chipModel()){
		case BME280::ChipModel_BME280:
		Serial.println("Found BME280 sensor! Success.");
		break;
		case BME280::ChipModel_BMP280:
		Serial.println("Found BMP280 sensor! No Humidity available.");
		break;
		default:
		Serial.println("Found UNKNOWN sensor! Error!");
	}
}

int dist;
float temp,pres,alt,hum;

void loop() {
	dist=getDist(trigPin,echoPin);
	tandp.read(pres, temp, hum, tempUnit, presUnit);
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

    Serial.print("Humidity = ");
    Serial.print(hum); // this should be adjusted to your local forcase
    Serial.println(" %");
}

void espuSerial(){
	Serial1.print("d:");
	Serial1.print(dist);
}