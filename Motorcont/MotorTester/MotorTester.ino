#include<ESP32Servo.h>

const int potPin=34;
const int cntlPin=11;
const int motorPin[]={33,32,23,19};

const int noMotors=4;
const int adcMax=4095;

const int lbnd[]={1735,1605,1605,1615};
const int ubnd[]={1850,2150,1710,2150};

Servo motor[noMotors];

int i;

void resetMotors();

void setup() {
  // put your setup code here, to run once:
  pinMode(cntlPin,INPUT_PULLUP);
  pinMode(potPin,INPUT);
  for(i=0;i<noMotors;i++){
    motor[i].attach(motorPin[i]);
  }
  resetMotors();
  for(i=0;i<noMotors;i++){
    motor[i].writeMicroseconds(lbnd[i]);
  }
  delay(5000);
//  Serial.begin(9600);
}
long throtle,tempMap,potVal;
void loop() {
  // put your main code here, to run repeatedly:
  potVal=analogRead(potPin);
  throtle=map(throtle,0,adcMax,0,100);
  if(digitalRead(cntlPin)){
    resetMotors();
  }
  else{
    for(i=0;i<noMotors;i++){
      tempMap=map(potVal,0,adcMax,lbnd[i],ubnd[i]);
      motor[i].writeMicroseconds(tempMap);
    }
  }
//  Serial.println();
//  Serial.println(bl);
//  Serial.println(bll)..;
  delay(200);
}

void resetMotors(){
  for(i=0;i<noMotors;i++){
    motor[i].writeMicroseconds(1000);
  }
  delay(2000);
}
