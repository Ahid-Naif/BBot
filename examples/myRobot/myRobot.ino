#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <BBot.h>
#include <string.h>

int motorA1 = 2, motorA2 = 3, motorB1 = 4, motorB2 = 5;

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
  myRobot.setupMotors(motorA1, motorA2, motorB1, motorB2);
  myRobot.setMode(Teleoperation);
  Serial.begin(115200); Serial.flush(); Serial.setTimeout(0);
  SPI.begin(); rfid.PCD_Init();
}

void loop(){
  //@startContainer
  myRobot.prepareForMovement();
  myRobot.movement();
  //@newContent
  //@endContainer
}
