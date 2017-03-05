#include <SoftwareSerial.h>
#include <BBot.h>
#include <string.h>

int motorA1 = 9, motorA2 = 8, motorB1 = 11, motorB2 = 12;
int redLED = 2, greenLED = 3, buzzer= 4;
int trig = 6, echo = 7;
String str;
//Instance of classes
SoftwareSerial serial(2,3);
BBot myRobot(serial, Teleoperation);

void serialEvent(){
  while(Serial.available()) {
    str = Serial.readString();
    String operation = myRobot.getValueFromString(str, ':', 0);
    String action = operation.substring(0);
    if(action == "V"){ //velocity
      myRobot.teleoperation(myRobot.getValueFromString(str, ':', 1).toInt(),myRobot.getValueFromString(str, ':', 2).toInt());
    }else if(action == "S"){ //start
      myRobot.isActive = true;
      dw(redLED,0);
      dw(greenLED,1);
    }else if(action == "P"){ //pause
      dw(redLED,1);
      dw(greenLED,0);
      myRobot.isActive = false;
    }
  }
}

void setup() {
  pinMode(motorA1,OUTPUT); pinMode(motorA2,OUTPUT); pinMode(motorB1,OUTPUT); pinMode(motorB2,OUTPUT);
  pinMode(redLED, OUTPUT); pinMode(greenLED, OUTPUT); pinMode(buzzer, OUTPUT); dw(redLED, 0); dw(greenLED, 0); dw(buzzer, 0);
  pinMode(echo, INPUT); pinMode(trig, OUTPUT);
  myRobot.setupMotors(motorA1, motorA2, motorB1, motorB2);
  myRobot.setupUltrasonic(trig, echo);
  myRobot.setupCircuitComponents(redLED, greenLED, buzzer);
  myRobot.setMode(Teleoperation);
  Serial.begin(115200); Serial.flush(); Serial.setTimeout(0);
}

void loop(){
  if(myRobot.isActive){
    if(myRobot.distanceFromUltrasonic() > 9 && myRobot.distanceFromUltrasonic() < 40){
      Serial.print("F");
      myRobot.isActive = !myRobot.isActive;
      for(int i = 0 ; i < 3 ; i++){
        dw(greenLED, 0);
        dw(redLED, 1);
        dw(buzzer, 1);
        delay(50);
        dw(redLED, 0);
        dw(buzzer, 0);
        delay(50);
        dw(redLED, 1);
      }
    }
  }else{
    myRobot.teleoperation(0,0);
  }
  myRobot.movement();
  delay(10);
}
