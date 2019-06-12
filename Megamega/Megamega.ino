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
  const int ibattpin=A0;
  const int ledpwmpin=4;
  const int nasesuncepin=25;
  const int chargercurrpin=A15;
  const int chargercurrmax=5;
  
const int nouss=sizeof(trigPin)/sizeof(int);

const int loopdelay=1000;

BME280I2C tandp;
BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
BME280::PresUnit presUnit(BME280::PresUnit_Pa);

int i;

void setup() {
  pinMode(buzzpin,OUTPUT);
  pinMode(ibattpin,INPUT);
  pinMode(ledpwmpin,OUTPUT);
  pinMode(nasesuncepin,OUTPUT);
  pinMode(chargercurrpin,INPUT);
  
  for(i=0;i<nouss;i++){
    pinMode(trigPin[i], OUTPUT);
    pinMode(echoPin[i], INPUT);
  }
  Serial1.begin(9600);
  Serial.begin(9600);
  Wire.begin();
  tempsens.begin();
  if(!tandp.begin())
    Serial.println("BME280 error!");
//  for(i=0;i<220;i++){
//    analogWrite(ledpwmpin,i);
//    delay(100);
//  }
  analogWrite(ledpwmpin,220);
  digitalWrite(nasesuncepin,0);
}

int dist[nouss];
bool charging=false;
float temp,pres,alt,hum;
double dstemp=0,ttemp,batcurr,chargercurr;

void loop() {
  charging=ischarging(chargercurrpin);
  Serial.println(charging);
//  for(i=0;i<nouss;i++){
//    dist[i]=getDist(trigPin[i],echoPin[i]);
//  }
//  tandp.read(pres, temp, hum, tempUnit, presUnit);
//  dsttemp();
//  pisci();
//  namaSerial();
//  espuSerial();
//  delay(loopdelay);
}

void dsttemp(){
  tempsens.requestTemperatures();
  ttemp=tempsens.getTempCByIndex(0);
  if(ttemp!=-127)
    dstemp=ttemp;
}

bool ischarging(const int pin){
  int t1,t2,t3;
  t1=analogRead(pin);
  delay(5);
  t2=analogRead(pin);
  delay(5);
  t3=analogRead(pin);
  return abs(t1-t2)+abs(t2-t3)>2;
}

double currmet(const int pin, int I){
  int raw=analogRead(pin);
  double calc;
  calc=raw-511;
  calc=abs(calc);
  calc=map(calc,0,511,0.0,I);
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
  Serial1.println(dist[0]);
}
