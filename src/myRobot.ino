#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <BBot.h>
#include <TimerOne.h>
#include <string.h>

//pins
int motorA1 = 6;
int motorA2 = 7;
int motorB1 = 9;
int motorB2 = 8;
int echoUp = 7;
int trigUp = 6;
int echoRight = 12;
int trigRight = 13;
int echoLeft = 10;
int trigLeft = 11;
int SS_PIN = 53;
int RST_PIN = 5;

//constants & variables
int counterThreshold = 5;
int balloonCounter = 0;

String serialString;
MFRC522 rfid(SS_PIN, RST_PIN);
BBot myRobot(rfid);

void serialEvent() {
  while( Serial.available() ) {
    serialString = Serial.readString();
    myRobot.performActionWithSerial(serialString);
  }
}

void setup() {
  //pins
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(echoUp, INPUT);
  pinMode(trigUp, OUTPUT);
  pinMode(echoRight, INPUT);
  pinMode(trigRight, OUTPUT);
  pinMode(echoLeft, INPUT);
  pinMode(trigLeft, OUTPUT);
  //bbot
  myRobot.setMode(Teleoperation);
  myRobot.setupMotors(motorA1, motorA2, motorB1, motorB2);
  myRobot.setupUltrasonics(trigUp, echoUp, trigRight, echoRight, trigLeft, echoLeft);
  //serial
  Serial.begin(115200);
  Serial.flush();
  Serial.setTimeout(0);
  //rfid
  SPI.begin();
  rfid.PCD_Init();
  //timer
  Timer1.initialize(100000);
  Timer1.attachInterrupt( timerCallback );
}

void loop() {
  //@startContainer
  myRobot.prepareForMovement();
  myRobot.movement();
  //@newContent
  //@endContainer
}

void timerCallback() {
  //myRobot.timerCallback();
  if(myRobot.goalMode != Teleoperation || !myRobot.isActive) { return ; }

  if(myRobot.distanceFromUltrasonic(BackUltrasonic) < 40){
    balloonCounter = 0;
  }else {
    balloonCounter++;
  }

  if(balloonCounter > counterThreshold) {
    balloonCounter = 0;
    Serial.print("F");
    myRobot.isActive = false;
  }
}
