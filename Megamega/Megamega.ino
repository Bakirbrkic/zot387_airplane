#include <BME280I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 53

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempsens(&oneWire);

double getDist(const int, const int);
void namaSerial();
void espuSerial();
void pisci();

const int trigPin[]={35,32};
const int echoPin[]={34,33};
const int buzzpin=13;
const int ibattpin=A15;
const int ledpwmpin=2;

const int nouss=sizeof(trigPin)/sizeof(int);

const int loopdelay=1000;

BME280I2C tandp;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

int i;

void setup() {
  for(i=0;i<nouss;i++){
    pinMode(trigPin[i], OUTPUT);
    pinMode(echoPin[i], INPUT);
  }
  Serial1.begin(115200);
  Serial.begin(9600);
  Wire.begin();
  tempsens.begin();
  if(!tandp.begin())
    Serial.println("BME280 error!");
}

int dist[nouss];
float temp,pres,alt,hum;
double dstemp,batcurr;

void loop() {
  for(i=0;i<nouss;i++){
    dist[i]=getDist(trigPin[i],echoPin[i]);
  }
  tandp.read(pres, temp, hum, tempUnit, presUnit);
  tempsens.requestTemperatures();
  dstemp=tempsens.getTempCByIndex(0);
  pisci();
  namaSerial();
  espuSerial();
  delay(loopdelay);
}

double currmet(const int pin, int I){
  int raw=analogRead(pin);
  double calc;
  calc=raw-511;
  calc=abs(calc);
  calc=map(calc,0,511,0,I);
  return calc;
}

void pisci(){
  tone(buzzpin,2000,100);
}

double getDist(const int tp, const int ep){
  double duration;
  digitalWrite(tp, LOW);
  delayMicroseconds(2);
  digitalWrite(tp, HIGH);
  delayMicroseconds(10);
  digitalWrite(tp, LOW);
  duration = pulseIn(ep, HIGH);
  return duration*0.034/2;
}

void namaSerial(){
  for(i=0;i<nouss;i++){
    Serial.print("Distance"+String(i)+" = ");
    Serial.print(dist[i]);
    Serial.println(" cm");
  }

  Serial.print("DSTemp = ");
  Serial.print(dstemp);
  Serial.println("°C");
  
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println("°C");
  
  Serial.print("Pressure = ");
  Serial.print(pres);
  Serial.println(" Pa");

  Serial.print("Humidity = ");
  Serial.print(hum); // this should be adjusted to your local forcase
  Serial.println(" %");
  Serial.println();
}

void espuSerial(){
  Serial1.print("d:");
  Serial1.print(dist[0]);
}
