/*
  ========================
  Rachis Systems Sdn. Bhd.
  ========================
      R & D Department
  ========================
          B-Bot
  ========================
  The B-Bot aims to provide a useful complementary tool for STEM education through robotics.
  The invention resides broadly in a flexible and modular robot that can be built by children
  using the different parts that are provided with the kit. It is an adaptable platform that
  takes into consideration the age and learning capabilities of a child. The robot also comes
  with a wide range of sensors, actuators and a graphical programming softwares to program the robot.
  The B-Bot aims at providing an engaging complementary tool to the current methods of
  learning by providing students with a hands-on learning experience in the field of
  Science, Technology, Engineering and Mathematics.
  ==========================
  ->B-Bot Library
  ->github:
  ->website: https://www.rachissystems.com
  ==========================
  Important Note:
  This is the original library for B-Bot product, it is an open-source library for NON-COMMERCIAL use only.
  Feel free to pull any request or issue you find to improve it :)
  ==========================
  ->->->->-><-<-<-<-<-<-<-<-
      Copyright© - 2016
     All Rights Reserved
  ->->->->-><-<-<-<-<-<-<-<-
*/

/*
--- RFID programming ---
green led on = 632
red led on = 374
buzzer on = 508
green led off = 388
red led off = 682
buzzer off = 613
ultrasonic = 393
seven segment = 342
delay 1s = 263
delay 2s = 462
if statement = 722
reset = 422
upload = 487

--- RFID1 ---
stop = 514
speed up = 763
slow down = 271
go left = 685, 692
go right = 512, 764, 407
forward = 604

--- Serial communication ---
## To the app:
  C1: card read
  F: finish/reach distination

## From the app:
  S: start
  P: pause
  V: velocity using teleoperation with x and y axis
  T: swetch to teleoperation mode
  A: swetch to autonomous mode(LineFollowing)
  L: you lost
  W: you win
*/

#ifndef BBot_h
#define BBot_h

#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config/global.h"
/**
  ## Available Modes:
  ------------------
  1. Teleoperation
  2. RFID level 1
  3. RFID level 2
  4. RFID programming
  5. Pure line-following
*/
enum Mode {
  Teleoperation,
  RFID1,
  RFID2,
  RFIDProgramming,
  LineFollowing
};
/**
  ## Available Actions [needed with RFID 1 & RFID 2]
  ------------------
  1. Right
  2. Left
  3. SpeedUp
  4. SlowDown
  5. Stop
*/
enum Action{
  Right,
  Left,
  SpeedUp,
  SlowDown
};
/**
> BBot main Class
@params
  when an instance object created
  a reference of SoftwareSerial
  object should be send.
*/
class BBot{
  public:
    BBot(SoftwareSerial& serial, Mode mode);
    void ResetEveryThing(void);
    /**
      setMode function:
      @params:
      mode: the needed mode to be set should be within the list of available modes in the enum above.
      @return:
      void
      ----------
      Can be called at any time or place.
    */
    void setMode(Mode mode);
    Mode getMode(void);
    /**
      setup functions:
    */
    void setupMotors(int motorA1, int motorA2, int motorB1, int motorB2);
    void setupIRs(int leftIR, int middleIR, int rightIR);
    void setupSevenSegment(int a, int b, int c, int d, int e, int f, int g);
    void setupUltrasonic(int trig, int echo);
    void setupCircuitComponents(int redLED, int greenLED, int buzzer);
    /**
      IRs function:
      @params:
      values: values of the connected IRs (left, middle, right)
      @return:
      void
      ----------
      Works with:
      1. Line follwing
      2. RFID1
      3. RFID2
      4. RFID programming
    */
    void IRs();
    /**
      RFID function:
      @params:
      cardId: the sum of the read card's bytes
      @return:
      void
      ----------
      Works with:
      1. RFID1
      2. RFID2
      3. RFID programming
    */
    void RFID(int cardId);
    void linkCurrentCardWithAction(String cardAction);
    /**
      teleoperation function:
      this function takes the rotation of the x any y axises from the mobile app based on
      the communication protocol. The minimum value for x and y axises is -45 degree and
      the maximum is 45 degree, however, it will affect the values of _linearVelocity, _angularVelocity only.
      @params:
      xAxis(int), yAxis(int)
      @return:
      void
      ----------
      Works with:
      1. Teleoperation
    */

    bool countOnSevenSegment(int from, int to, int delayDuration);
    void setNumberOnSevenSegment(int number);
    void teleoperation(int xAxis,int yAxis);
    /**
      movement function:
      @params:
      void
      @return:
      void
      ----------
      Works with:
      1. Line follwing
      2. RFId1
      3. RFID2
      4. RFID programming
      5. Teleoperation
      6. Controllable
    */
    void movement(void);
    bool goToTheRight(void);
    bool goToTheLeft(void);
    float distanceFromUltrasonic();
    String getValueFromString(String data, char separator, int index);
    bool isActive;
  private:
    void stopForever(void);
    bool isRegisteredIn(int cardId, String action);
    /**
      _serial: the reference of the initialized object from SoftwareSerial class, used in the communication between the app and the robot.
    */
    SoftwareSerial& _serial;
    /*
      * isReady: block the robot from its movement and wait for activation code from the app or any custom event.
      * _motorA1:
      * _motorA2:
      * _motorB1:
      * _motorB2:
      * _IR1:
      * _IR2:
      * _IR3:
      * _linearVelocity:
      * _angularVelocity:
      * _velocityLeftMotor:
      * _velocityRightMotor:
    */
    int _motorA1, _motorA2, _motorB1, _motorB2;
    int _IR1, _IR2, _IR3;
    int _a, _b, _c, _d, _e, _f, _g; //seven segment
    int _trig, _echo;
    int _greenLED, _redLED, _buzzer;
    double _linearVelocity, _angularVelocity, _velocityLeftMotor, _velocityRightMotor;
    int numberOfCardCanBeReadRFID1;
    int rightCards[10];
    int leftCards[10];
    int stopCard;
    Mode mode, goalMode;
    Action action;
    int currentCardId;
    bool doneSetup;
};

#endif
