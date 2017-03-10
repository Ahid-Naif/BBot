#include "BBot.h"

//group all constants together.
const struct {
  int xAngleMax = 45;
  int xAngleMin = -45;
  int yAngleMax = 45;
  int yAngleMin = -45;
  int omegaMax = 255;
  int omegaMin = -255;
  int velocityMax = 255;
  int velocityMin = -255;
  double v_move = 160;
  int w_turn = 70;
  int w_sharpTurn = 100;
  double b = 1;
} constants;

// Public methods
BBot::BBot(SoftwareSerial& serial, Mode mode): _serial(serial){
  this->_serial = serial;
  this->_angularVelocity = 0;
  this->_linearVelocity = 0;
  this->goalMode = mode;
  this->numberOfCardCanBeRead = 9;
  this->currentCardId = 0;
  this->doneSetup = false;
  this->rightCards[10] = {0};
  this->leftCards[10] = {0};
}

void BBot::ResetEveryThing(){
  this->numberOfCardCanBeRead = 9;
  this->currentCardId = 0;
  this->setNumberOnSevenSegment(-1);
  dw(this->_buzzer,0);
  dw(this->_greenLED,0);
  dw(this->_redLED,0);
}

void BBot::setMode(Mode mode){
  this->mode = mode;
}

Mode BBot::getMode(){
  return this->mode;
}

void BBot::setupMotors(int motorA1, int motorA2, int motorB1, int motorB2){
  this->_motorA1 = motorA1;
  this->_motorA2 = motorA2;
  this->_motorB1 = motorB1;
  this->_motorB2 = motorB2;
}

void BBot::setupIRs(int leftIR, int middleIR, int rightIR){
  this->_IR1 = leftIR;
  this->_IR2 = middleIR;
  this->_IR3 = rightIR;
}

void BBot::setupSevenSegment(int a, int b, int c, int d, int e, int f, int g){
  this->_a = a;
  this->_b = b;
  this->_c = c;
  this->_d = d;
  this->_e = e;
  this->_f = f;
  this->_g = g;
  this->setNumberOnSevenSegment(-1);
}

void BBot::setupUltrasonic(int trig, int echo){
  this->_trig = trig;
  this->_echo = echo;
}

void BBot::setupCircuitComponents(int redLED, int greenLED, int buzzer){
  this->_buzzer = buzzer;
  this->_greenLED = greenLED;
  this->_redLED = redLED;
}

int lastStateOfLineFollowing = 0;
void BBot::IRs(){
  if(this->mode != LineFollowing) {return;}
  int leftIR = digitalRead(this->_IR1);
  int middleIR = digitalRead(this->_IR2);
  int rightIR = digitalRead(this->_IR3);
  if( //forward
    ( leftIR && !middleIR && rightIR )
    ||
    ( !leftIR && middleIR && !rightIR )
    ||
    ( leftIR && middleIR && rightIR )
  ){
    this->_linearVelocity = constants.v_move;
    this->_angularVelocity = 0;
  }else if( (leftIR == 1) && (middleIR == 1) && (rightIR == 0) ){ //Left
    this->_linearVelocity = constants.v_move/2;
    this->_angularVelocity = constants.w_turn;
  }else if( (leftIR == 1) && (middleIR == 0) && (rightIR == 0) ){ //SharpLeft
    lastStateOfLineFollowing = 1; //SharpLeft
    this->_linearVelocity = 0;
    this->_angularVelocity = constants.w_sharpTurn;
  }else if( (leftIR == 0) && (middleIR == 1) && (rightIR == 1) ){ //Right
    this->_linearVelocity = constants.v_move/2;
    this->_angularVelocity = -constants.w_turn;
  }else if( (leftIR == 0) && (middleIR == 0) && (rightIR == 1) ){ //SharpRight
    lastStateOfLineFollowing = 2; //SharpRight
    this->_linearVelocity = 0;
    this->_angularVelocity = -constants.w_sharpTurn;
  }else if( !leftIR && !middleIR && !rightIR ){
    if(lastStateOfLineFollowing == 1){
      this->_linearVelocity = constants.w_sharpTurn/2;
      this->_angularVelocity = constants.w_sharpTurn/2;
    }else if(lastStateOfLineFollowing == 2){
      this->_linearVelocity = constants.w_sharpTurn/2;
      this->_angularVelocity = -constants.w_sharpTurn/2;
    }
  }
}

void BBot::RFID(int cardId){
  if(!this->isActive){ return; }
  int numberOfPulses = 1;
  if(this->goalMode == RFID1 && this->mode == LineFollowing){
    this->mode = RFID1;
    if(cardId == 514){
      Serial.print("F");
      this->isActive = false;
      this->mode = LineFollowing;
      numberOfPulses = 3;
    }else if( cardId == 512 || cardId == 764 || cardId == 407 ){
      this->action = Right;
      Serial.print("C1");
    }else if( cardId == 685 || cardId == 692){
      this->action = Left;
      Serial.print("C1");
    }else if( cardId == 763){
      this->action = SpeedUp;
      Serial.print("C1");
    }else if( cardId == 271){
      this->action = SlowDown;
      Serial.print("C1");
    }else{
      Serial.print("C1");
    }
    for (int i = 0; i < numberOfPulses; i++) {
      dw(this->_greenLED, 1);
      dw(this->_buzzer, 1);
      delay(50);
      dw(this->_greenLED, 0);
      dw(this->_buzzer, 0);
      delay(50);
    }
    this->setNumberOnSevenSegment(numberOfCardCanBeRead);
    numberOfCardCanBeRead--;
    if(numberOfCardCanBeRead == -1){
      dw(this->_redLED,1);
    }
  }else if(this->goalMode == RFID2){
    if(this->mode == LineFollowing && this->doneSetup == false){ //storing: CW:100 -> current && !active
      this->isActive = false;
      this->currentCardId = cardId;
      Serial.print("CW:"+(String)cardId);
    }else if(this->mode == LineFollowing && this->doneSetup == true){ //run as RFID1
      //add to the list
      this->mode = RFID2;
      if(cardId == this->stopCard){
        Serial.print("F");
        this->isActive = false;
        numberOfPulses = 3;
      }else if( this->isRegisteredIn(cardId, "Right") ){
        this->action = Right;
        Serial.print("C1");
      }else if( this->isRegisteredIn(cardId, "Left") ){
        this->action = Left;
        Serial.print("C1");
      }else if( cardId == 763){
        //this->action = SpeedUp;
        Serial.print("C1");
      }else if( cardId == 271){
        //this->action = SlowDown;
        Serial.print("C1");
      }else{
        Serial.print("C1");
      }
      for (int i = 0; i < numberOfPulses; i++) {
        dw(this->_greenLED, 1);
        dw(this->_buzzer, 1);
        delay(50);
        dw(this->_greenLED, 0);
        dw(this->_buzzer, 0);
        delay(50);
      }
      this->setNumberOnSevenSegment(numberOfCardCanBeRead);
      numberOfCardCanBeRead--;
      if(numberOfCardCanBeRead == -1){
        dw(this->_redLED,1);
      }
    }
  }else if(this->goalMode == RFIDProgramming){
    if(cardId == 487){
      this->isActive = false;
    }
    Serial.print(cardId);
  }
}

void BBot::linkCurrentCardWithAction(String cardAction){
  this->isActive = true;
  if(cardAction == "R"){ //link the current card with the needed action
    for (int i = 0; i < 10; i++) {
      if(this->rightCards[i] == 0){
        this->rightCards[i] = this->currentCardId;
        break;
      }
    }
  }else if(cardAction == "L"){
    for (int i = 0; i < 10; i++) {
      if(this->leftCards[i] == 0){
        this->leftCards[i] = this->currentCardId;
        break;
      }
    }
  }else if(cardAction == "S"){
    this->stopCard = this->currentCardId;
    this->doneSetup = true;
    this->isActive = false;
  }else if(cardAction == "F"){

  }else if(cardAction == "SU"){

  }else if(cardAction == "SL"){

  }
}

bool BBot::countOnSevenSegment(int from, int to, int delayDuration){
  for (int i = from ; i <= to ; i++) {
    this->setNumberOnSevenSegment(i);
    delay(delayDuration);
  }
  return true;
}

void BBot::setNumberOnSevenSegment(int number){
  switch (number) {
    case 0:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 0);
    dw(this->_f, 0);
    dw(this->_g, 1);
    break;
    case 1:
    dw(this->_a, 1);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 1);
    dw(this->_e, 1);
    dw(this->_f, 1);
    dw(this->_g, 1);
    break;
    case 2:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 1);
    dw(this->_d, 0);
    dw(this->_e, 0);
    dw(this->_f, 1);
    dw(this->_g, 0);
    break;
    case 3:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 1);
    dw(this->_f, 1);
    dw(this->_g, 0);
    break;
    case 4:
    dw(this->_a, 1);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 1);
    dw(this->_e, 1);
    dw(this->_f, 0);
    dw(this->_g, 0);
    break;
    case 5:
    dw(this->_a, 0);
    dw(this->_b, 1);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 1);
    dw(this->_f, 0);
    dw(this->_g, 0);
    break;
    case 6:
    dw(this->_a, 0);
    dw(this->_b, 1);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 0);
    dw(this->_f, 0);
    dw(this->_g, 0);
    break;
    case 7:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 1);
    dw(this->_e, 1);
    dw(this->_f, 1);
    dw(this->_g, 1);
    break;
    case 8:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 0);
    dw(this->_f, 0);
    dw(this->_g, 0);
    break;
    case 9:
    dw(this->_a, 0);
    dw(this->_b, 0);
    dw(this->_c, 0);
    dw(this->_d, 0);
    dw(this->_e, 1);
    dw(this->_f, 0);
    dw(this->_g, 0);
    break;
    case -1:
    dw(this->_a, 1);
    dw(this->_b, 1);
    dw(this->_c, 1);
    dw(this->_d, 1);
    dw(this->_e, 1);
    dw(this->_f, 1);
    dw(this->_g, 1);
    break;
  }
}

void BBot::teleoperation(int xAxis, int yAxis){
  this->_linearVelocity =  constrain(xAxis, constants.xAngleMin, constants.xAngleMax);
  this->_linearVelocity = map(this->_linearVelocity, constants.xAngleMin, constants.xAngleMax, constants.velocityMin, constants.velocityMax);
  this->_angularVelocity = constrain(yAxis, constants.yAngleMin, constants.yAngleMax);
  this->_angularVelocity = map(this->_angularVelocity, constants.yAngleMin, constants.yAngleMax, constants.omegaMin, constants.omegaMax);
}

void BBot::movement(){
  if((this->mode == RFID1 || this->mode == RFID2) && this->isActive){
    switch (this->action) {
      case Right:
      this->teleoperation(0,-30);
      if(
        (!dr(this->_IR1) && !dr(this->_IR2) && dr(this->_IR3))
        ||
        (!dr(this->_IR1) && dr(this->_IR2) && dr(this->_IR3))
      ){
        this->mode = LineFollowing;
      }
      break;
      case Left:
      this->teleoperation(0,30);
      if(
        (dr(this->_IR1) && !dr(this->_IR2) && !dr(this->_IR3))
        ||
        (dr(this->_IR1) && dr(this->_IR2) && !dr(this->_IR3))
      ){
        this->mode = LineFollowing;
      }
      break;
      case SpeedUp:
        this->mode = LineFollowing;
      break;
      case SlowDown:
        this->mode = LineFollowing;
      break;
    }
  }
    this->_velocityRightMotor = constrain((this->_linearVelocity - constants.b * this->_angularVelocity),-255,255);
    this->_velocityLeftMotor = constrain((this->_linearVelocity + constants.b * this->_angularVelocity),-255,255);
    if(this->_velocityRightMotor<0){
        analogWrite(this->_motorA1,abs(this->_velocityRightMotor));
        analogWrite(this->_motorA2,0);
    }else{
        analogWrite(this->_motorA1,0);
        analogWrite(this->_motorA2,this->_velocityRightMotor);
    }
    if(this->_velocityLeftMotor<0){
        analogWrite(this->_motorB1,abs(this->_velocityLeftMotor));
        analogWrite(this->_motorB2,0);
    }else{
        analogWrite(this->_motorB1,0);
        analogWrite(this->_motorB2,this->_velocityLeftMotor);
    }
}

bool BBot::goToTheRight(void){
  this->teleoperation(0,-20);
  if(
    (!dr(this->_IR1) && !dr(this->_IR2) && dr(this->_IR3))
    ||
    (!dr(this->_IR1) && dr(this->_IR2) && dr(this->_IR3))
  ){
    return true;
  }
  return false;
}

bool BBot::goToTheLeft(void){
  this->teleoperation(0,20);
  if(
    (dr(this->_IR1) && !dr(this->_IR2) && !dr(this->_IR3))
    ||
    (dr(this->_IR1) && dr(this->_IR2) && !dr(this->_IR3))
  ){
    return true;
  }
  return false;
}

float BBot::distanceFromUltrasonic(){
  digitalWrite(this->_trig,LOW);
  delayMicroseconds(2);
  digitalWrite(this->_trig,HIGH);
  delayMicroseconds(7);
  digitalWrite(this->_trig,LOW);
  float time = pulseIn(this->_echo,HIGH);
  return time / 58;
}

String BBot::getValueFromString(String data, char separator, int index){
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
      found++;
      strIndex[0] = strIndex[1]+1;
      strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

// Private methods
void BBot::stopForever(){
  this->isActive = false;
}

bool BBot::isRegisteredIn(int cardId, String action){
  bool found = false;
  for(int i =0 ; i < 10; i++){
    if(action == "Right"){
      if(this->rightCards[i] == cardId){
        found = true;
        break;
      }
    }else if(action == "Left"){
      if(this->leftCards[i] == cardId){
        found = true;
        break;
      }
    }
  }
  return found;
}
