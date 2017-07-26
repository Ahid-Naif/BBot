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
  ->github: https://github.com/RachisSystems/B-Bot
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
  G: set the needed goal mode
  O: logic initial
    A: and operator
    O: or operator
*/

#ifndef BBot_h
#define BBot_h

#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include "config/global.h"
#include "Timer/Timer.h"
#include <iostream>
#include <map>

using namespace std;

/**
  ## Available Modes:
  ------------------
  1. Teleoperation
  2. RFID level 1 (cardGamer)
  3. RFID level 2 (cardProgrammer)
  4. RFID programming
  5. Pure line-following
  6. Obstacle avoidance
*/
enum Mode {
  Teleoperation, //1
  RFID1, //2
  RFID2, //3
  RFIDProgramming, //4
  LineFollowing,
  Logic, //5
  Loop, //6
  ObstacleAvoidance
};
/*
  Ultrasonics Positions

*/
enum Ultrasonics {
  ultraTop,
  ultraRight,
  ultraLeft
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

class BBot{
  public:
    /**
      BBot main Class (constructor)
      @params:
        when an instance object created a reference of MFRC522 object should be send.
    */
    BBot(MFRC522& rfid);
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
    /**
      getMode function:
      @params:
      void
      @return:
      Mode: the current mode stored in the object
      ----------
      Can be called at any time or place.
    */
    Mode getMode(void);
    /** SETUP FUNCTIONS **/
    /**
      setupMotors function:
      @params:
      motorA1: connected pin for motor A with first terminal
      motorA2: connected pin for motor A with second terminal
      motorB1: connected pin for motor B with first terminal
      motorB1: connected pin for motor B with second terminal
      @return:
      void
      ----------
      called in void setup function.
    */
    void setupMotors(int motorA1, int motorA2, int motorB1, int motorB2);
    /**
      setupIRs function:
         ---
       L/ M \R
        |   |
        |---|
      @params:
      leftIR: L.
      middleIR: M.
      rightIR: R.
      @return:
      void
      ----------
      called in void setup function to setup the three connected IRs sensors
      to the robot, which can be handy in this class to deal with.
    */
    void setupIRs(int leftIR, int middleIR, int rightIR);
    /**
      setupSevenSegment function:
        _         a
      |   |     f   b
        -   =>    g
      |   |     e   c
        -         d
      @params:
      a: digital pin connected with a on 7-segment
      b: digital pin connected with b on 7-segment
      c: digital pin connected with c on 7-segment
      d: digital pin connected with d on 7-segment
      e: digital pin connected with e on 7-segment
      f: digital pin connected with f on 7-segment
      g: digital pin connected with g on 7-segment
      @return:
      void
      ----------
      called in void setup function.
      feel free to use other method for 7-segment like i2c,
      we had chosen the simplest one for educational purpose.
    */
    void setupSevenSegment(int a, int b, int c, int d, int e, int f, int g);
    /**
      setupUltrasonic function:
      @params:
      trig: the digital pin that connected to trig terminal on the ultrasonic
      echo: the digital pin that connected to echo terminal on the ultrasonic
      @return:
      void
      ----------
      called in void setup function.
    */
    void setupUltrasonic(int trigUp, int echoUp, int trigRight, int echoRight,int trigLeft, int echoLeft);
    /**
      setupCircuitComponents function:
      @params:
      redLED: the digital pin that connected to the red LED.
      greenLED: the digital pin that connected to the green LED.
      buzzer: the digital pin that connected to the buzzer.
      @return:
      void
      ----------
      called in void setup function.
      MAKE SURE to limit the current by adding two resistors in series with the two red and green LEDs.
    */
    void setupCircuitComponents(int redLED, int greenLED, int buzzer);
    /**
      IRs function:
      @params:
      vodi
      @return:
      void
      ----------
      perform Line Follwing operation.
      make sure to switch to line following mode using setMode(_) function so that this function can work.
    */
    void IRs();
    /**
      RFID function:
      @params:
      void
      @return:
      void
      ----------
      if the goalMode set to RFID1(cardGamer): this function will work simultaneously with IRs(_) function
      where IRs(_) function will perform line following and whenever the robot read new RFID card the RFID(_)
      will be activiated and it will perform the command assigned with the card.
    */
    void RFID();
    /**
      linkCurrentCardWithAction function:
      @params:
      cardAction: string correspond to each command that needed to be assigned to current card.
      @return:
      void
      ----------
      link the current card with the needed action.
    */
    void linkCurrentCardWithAction(String cardAction);
    /**
      setNumberOnSevenSegment function:
      @params:
      number: the needed number to be shown on the 7-segment(0-9)
      @return:
      void
      ----------
      set the needed number on the 7-segment chip.
    */
    void setNumberOnSevenSegment(int number);
    /**
      countOnSevenSegment function:
      @params:
      from: starting point
      to: ending point
      delayDuration: the needed delay between each iteration
      @return:
      void
      ----------
      it uses setNumberOnSevenSegment(_) to count from-to specific numbers with given delay.
    */
    bool countOnSevenSegment(int from, int to, int delayDuration);
    /**
      teleoperation function:
      @params:
      xAxis(int), yAxis(int)
      @return:
      void
      ----------
      this function takes the rotation of the x any y axises from the mobile app based on
      the communication protocol. The minimum value for x and y axises is -45 degree and
      the maximum is 45 degree, however, it will affect the values of _linearVelocity, _angularVelocity only.
    */
    void teleoperation(int xAxis,int yAxis);
    /**
      movement function:
      @params:
      void
      @return:
      void
      ----------
      perform the actual movement of the robot.
    */
    void movement(void);
    /**
      distanceFromUltrasonic function:
      @params:
      void
      @return:
      float value of the measured distance from the ultrasonic.
      ----------
      perform the actual movement of the robot.
    */
    float distanceFromUltrasonic(Ultrasonics ultrasonics);
    /**
      ResetEveryThing function:
      @params:
      void
      @return:
      void
      ----------
      Turns off all LEDs, reset currentCardId to zero and it call setNumberOnSevenSegment(-1) to clear 7-segment output.
    */
    void ResetEveryThing(void);
    /**
      getValueFromString function:
      @params:
      data: string needed to be splitted.
      separator: the separator that the given string will be divided based on it.
      index: the index needed after executing the function.
      @return:
      void
      ----------
      handy function used to seprate(split) given string into several parts.
      example of usage:
      String str = "S:1";
      String operation = myRobot.getValueFromString(str, ':', 0); //return `S`
    */
    String getValueFromString(String data, char separator, int index);
    /*
      isActive is a tricky part of the logic because it used in many different cases,
      but it's pretty useful in most of the critical cases where you want to
      deactivate the movement for some seconds.
    */
    bool isActive;

    void performActionWithSerial(String str);
    void prepareForMovement(void);

    /*
      Timer
      Object timer to check ultrasonic status
      counter
    */
    Timer timer;
    int balloonCounter;
    void balloonCounterIncrement();

  private:
    /**
      stopForever function:
      @params:
      void
      @return:
      void
      ----------
      stop every thing in the robot.
    */
    void stopForever(void);
    /**

    */
    MFRC522& _rfid;
    /** other useful private variables **/
    int _motorA1, _motorA2, _motorB1, _motorB2;
    int _IR1, _IR2, _IR3;
    int _a, _b, _c, _d, _e, _f, _g; //seven segment
    int _trigUp, _echoUp, _trigRight, _echoRight, _trigLeft, _echoLeft; //ULTRASONIC
    int _greenLED, _redLED, _buzzer;
    /*
      _velocityLeftMotor and _velocityRightMotor: used after applying mathematical equation in movement(_)
      function where this values will assigned to the left and right motors respectively.
    */
    double _linearVelocity, _angularVelocity, _velocityLeftMotor, _velocityRightMotor;
    int iterations; // to specifies the maximum number of rounds for  "Loop" Mode
    int numberRounds; // it represents the current round number
    String and_or; // its value is either A which stands for "and", or O which stands for "or"
    /*
      numberOfCardCanBeRead: used to count how many cards left can be read before losing the match in RFID1 and RFID2.
    */
    int numberOfCardCanBeRead;
    Map<int, String> cards;
    Map<string, int> logicalCards; // this map is to read the cards of the "Logic" Mode
    /*
      the differance between mode and goalMode is that mode can be changed
      according to the need anytime but goalMode cannot be changed while the
      game is running where changing it can be done using the app only.
    */
    Mode mode, goalMode;
    /*
      ultrasonics enumeration
    */
    Ultrasonics ultrasonics;
    /*
      action variable used in RFID operation where it can be changed according
      to the action read on the card.
    */
    Action action;
    /*
      store the card's ID that has been read previously, it's useful to assign the
      command from the app to this card in linkCurrentCardWithAction(_) function.
    */
    int currentCardId;
    /*
      a boolean indicator used in RFID2 where its value changed to true when the
      command sent from the app telling it that the setup for the cards is done
      and ready to play the game on the programmed command on each card.
    */
    bool doneSetup;


};

#endif
