#include <Arduino.h>
#include<Servo.h>
Servo motor[4];
int i;
int potPin=A2;
int cntlPin=7;
const int noMotors=4;
const int motorPin[]={3,5,6,9};
const int lbnd[]={1735,1605,1605,1615};
const int ubnd[]={1850,2150,1710,2150};
void resetMotors();
void setup() {
  // put your setup code here, to run once:
  pinMode(cntlPin,INPUT_PULLUP);
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
  throtle=map(throtle,0,1023,0,100);
  if(digitalRead(cntlPin)){
    resetMotors();
  }
  else{
    for(i=0;i<noMotors;i++){
      tempMap=map(potVal,0,1023,lbnd[i],ubnd[i]);
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