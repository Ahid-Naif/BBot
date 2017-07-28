#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <BBot.h>
#include <TimerOne.h>
#include <string.h>

int motorA1 = 2, motorA2 = 3, motorB1 = 8, motorB2 = 9, balloonCounter = 0 , threshold = 5;
int echoUp = 7, trigUp = 6, echoRight = 12, trigRight = 13, echoLeft = 10, trigLeft = 11, timerInterval = 250;
String str;
#define SS_PIN 53
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);
BBot myRobot(rfid);


void serialEvent(){
  while(Serial.available()) {
    str = Serial.readString();
    myRobot.performActionWithSerial(str);
  }
}

void setup() {
  pinMode(motorA1,OUTPUT); pinMode(motorA2,OUTPUT); pinMode(motorB1,OUTPUT); pinMode(motorB2,OUTPUT);
  pinMode(echoUp, INPUT); pinMode(trigUp, OUTPUT);pinMode(echoRight, INPUT); pinMode(trigRight, OUTPUT); pinMode(echoLeft, INPUT); pinMode(trigLeft, OUTPUT);
  myRobot.setupMotors(motorA1, motorA2, motorB1, motorB2);
  myRobot.setMode(Teleoperation);
  myRobot.setupUltrasonic(trigUp, echoUp, trigRight, echoRight, trigLeft, echoLeft);
  Serial.begin(115200); Serial.flush(); Serial.setTimeout(0);
  SPI.begin(); rfid.PCD_Init();
  Timer1.initialize(5000);
  Timer1.attachInterrupt( balloonTimer );
}

void loop(){
  //@startContainer
  if(myRobot.isActive){
    if(balloonCounter ==3)
    Serial.println("zzzz");
    Serial.println(balloonCounter);

  }
  myRobot.prepareForMovement();
  myRobot.movement();
  //@newContent
  //@endContainer
}
void balloonTimer()
{
  if(myRobot.goalMode == Teleoperation && myRobot.isActive)
  {
    if(myRobot.distanceFromUltrasonic(ultraTop) < 40){
      balloonCounter = 0;
    }
    else{
      balloonCounter++;
    }

    if(balloonCounter > threshold)
      {
        Serial.print("F");
        myRobot.isActive = false;
      }
  }
}
