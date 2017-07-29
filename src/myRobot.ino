#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <BBot.h>
#include <TimerOne.h>
#include <string.h>

struct Pins {
  int motorA1 = 2;
  int motorA2 = 3;
  int motorB1 = 8;
  int motorB2 = 9;
  int echoUp = 7;
  int trigUp = 6;
  int echoRight = 12;
  int trigRight = 13;
  int echoLeft = 10;
  int trigLeft = 11;
  int SS = 53;
  int RST = 5;
} Pins;

struct Constants {
  int timerInterval = 5000;
  int counterThreshold = 5;
} Constants;

int balloonCounter = 0;
String str;
MFRC522 rfid(Pins.SS, Pins.RST);
BBot myRobot(rfid);


void serialEvent() {
  while( Serial.available() ) {
    str = Serial.readString();
    myRobot.performActionWithSerial(str);
  }
}

void setupPins() {
  pinMode(Pins.motorA1, OUTPUT);
  pinMode(Pins.motorA2, OUTPUT);
  pinMode(Pins.motorB1, OUTPUT);
  pinMode(Pins.motorB2, OUTPUT);
  pinMode(Pins.echoUp, INPUT);
  pinMode(Pins.trigUp, OUTPUT);
  pinMode(Pins.echoRight, INPUT);
  pinMode(Pins.trigRight, OUTPUT);
  pinMode(Pins.echoLeft, INPUT);
  pinMode(Pins.trigLeft, OUTPUT);
}

void setupBBot() {
  myRobot.setMode(Teleoperation);
  myRobot.setupMotors(
    Pins.motorA1,
    Pins.motorA2,
    Pins.motorB1,
    Pins.motorB2
  );
  myRobot.setupUltrasonic(
    Pins.trigUp,
    Pins.echoUp,
    Pins.trigRight,
    Pins.echoRight,
    Pins.trigLeft,
    Pins.echoLeft
  );
}

void setupSerial() {
  Serial.begin(115200);
  Serial.flush();
  Serial.setTimeout(0);
}

void setupRFID() {
  SPI.begin();
  rfid.PCD_Init();
}

void setupTimer() {
  Timer1.initialize(Constants.timerInterval);
  Timer1.attachInterrupt( timerCallback );
}

void setup() {
  setupPins();
  setupBBot();
  setupSerial();
  setupRFID();
  setupTimer();
}

void loop() {
  if(! myRobot.isActive) { return ; }

  //@startContainer
  myRobot.prepareForMovement();
  myRobot.movement();
  //@newContent
  //@endContainer
}

void timerCallback() {
  if(myRobot.goalMode != Teleoperation || ! myRobot.isActive) { return ; }

  if(myRobot.distanceFromUltrasonic(ultraTop) < 40){
    balloonCounter = 0;
  }else {
    balloonCounter++;
  }

  if(balloonCounter > Constants.counterThreshold) {
    Serial.print("F");
    myRobot.isActive = ! myRobot.isActive;
  }
}
