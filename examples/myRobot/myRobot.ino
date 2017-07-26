#include <SoftwareSerial.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Timer.h>
#include <BBot.h>
#include <string.h>

int ballonCounter = 0;
int timerInterval = 250;

const struct {
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
  int SS_PIN = 53;
  int RST_PIN = 5;
} Pins;

String str;
Timer timer;
MFRC522 rfid(Pins.SS_PIN, Pins.RST_PIN);
BBot myRobot(rfid);

//setup functions
void setupPins(){
  pinMode(Pins.motorA1,OUTPUT);
  pinMode(Pins.motorA2,OUTPUT);
  pinMode(Pins.motorB1,OUTPUT);
  pinMode(Pins.motorB2,OUTPUT);

  pinMode(Pins.echoUp, INPUT);
  pinMode(Pins.trigUp, OUTPUT);
  pinMode(Pins.echoRight, INPUT);
  pinMode(Pins.trigRight, OUTPUT);
  pinMode(Pins.echoLeft, INPUT);
  pinMode(Pins.trigLeft, OUTPUT);
}

void setupBBot(){
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

void serialEvent(){
  while(Serial.available()) {
    str = Serial.readString();
    myRobot.performActionWithSerial(str);
  }
}

void setupSerial(){
  Serial.begin(115200);
  Serial.flush();
  Serial.setTimeout(0);
}

void setupRFID(){
  SPI.begin();
  rfid.PCD_Init();
}

void timerCallback(){
  myRobot.timerCallback();
}

void setup() {
  setupPins();
  setupBBot();
  setupSerial();
  setupRFID();
  timer.every(timerInterval, timerCallback);
}

void loop(){
  if(! myRobot.isActive){ return; }
  if(myRobot.isTimerNeeded()){ timer.update(); }

  //@startContainer
  myRobot.prepareForMovement();
  myRobot.movement();
  //@newContent
  //@endContainer
}
