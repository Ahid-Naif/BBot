#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <BBot.h>
#include <string.h>
int leftIR = 37, middleIR = 38, rightIR = 39;
int motorA1 = 9, motorA2 = 8, motorB1 = 10, motorB2 = 11;
int redLED = 2, greenLED = 3, buzzer= 4;
int a = 46, b = 47, c = 43, d = 45, e = 44, f = 49, g = 48;
int trig = 6, echo = 7;
String str;
#define SS_PIN 53
#define RST_PIN 5
//Instance of classes
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
SoftwareSerial serial(2,3);
BBot myRobot(serial, RFID1);

void RFID(){
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) { return; }
  int cardId = rfid.uid.uidByte[0] + rfid.uid.uidByte[1] + rfid.uid.uidByte[2] + rfid.uid.uidByte[3];
  myRobot.RFID(cardId);
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

void serialEvent(){
  while(Serial.available()) {
    str = Serial.readString();
    String operation = myRobot.getValueFromString(str, ':', 0);
    String action = operation.substring(0);
    if(action == "S"){
      myRobot.ResetEveryThing();
      myRobot.isActive = true;
    }else if(action == "P"){
      myRobot.isActive = false;
    }else if(action == "T"){
      myRobot.setMode(Teleoperation);
    }else if(action == "A"){
      myRobot.setMode(LineFollowing);
    }else if(action == "V"){
      myRobot.teleoperation(myRobot.getValueFromString(str, ':', 1).toInt(),myRobot.getValueFromString(str, ':', 2).toInt());
    }
  }
}

void setup() {
  pinMode(leftIR,INPUT); pinMode(middleIR,INPUT); pinMode(rightIR,INPUT);
  pinMode(motorA1,OUTPUT); pinMode(motorA2,OUTPUT); pinMode(motorB1,OUTPUT); pinMode(motorB2,OUTPUT);
  pinMode(redLED, OUTPUT); pinMode(greenLED, OUTPUT); pinMode(buzzer, OUTPUT); dw(redLED, 0); dw(greenLED, 0); dw(buzzer, 0);
  pinMode(a, OUTPUT); pinMode(b, OUTPUT); pinMode(c, OUTPUT); pinMode(d, OUTPUT); pinMode(e, OUTPUT); pinMode(f, OUTPUT); pinMode(g, OUTPUT);
  pinMode(echo, INPUT); pinMode(trig, OUTPUT);
  myRobot.setupMotors(motorA1, motorA2, motorB1, motorB2);
  myRobot.setupIRs(leftIR, middleIR, rightIR);
  myRobot.setupSevenSegment(a,b,c,d,e,f,g);
  myRobot.setupUltrasonic(trig, echo);
  myRobot.setupCircuitComponents(redLED, greenLED, buzzer);
  myRobot.setMode(LineFollowing);
  Serial.begin(115200); Serial.flush(); Serial.setTimeout(0);
  SPI.begin(); rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) { key.keyByte[i] = 0xFF; }
}

void loop(){
  if(myRobot.isActive){
    RFID();
    myRobot.IRs();
  }else{
    myRobot.teleoperation(0,0);
  }
  myRobot.movement();
  delay(10);
}
