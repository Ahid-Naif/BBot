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
  double v_move = 150;
  double b = 1;
} constants;

// Public methods
BBot::BBot(MFRC522& rfid): _rfid(rfid){
  // this->_rfid = rfid;
  this->_angularVelocity = 0;
  this->_linearVelocity = 0;
  this->currentCardId = 0;
  this->doneSetup = false;
}

void BBot::resetEveryThing(){
  this->cards.clear();
  this->currentCardId = 0;
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

void BBot::setupUltrasonics(int trigUp, int echoUp, int trigRight, int echoRight,int trigLeft, int echoLeft){
  this->_backUltrasonicTrig = trigUp;
  this->_backUltrasonicEcho = echoUp;
  this->_rightUltrasonicTrig = trigRight;
  this->_rightUltrasonicEcho = echoRight;
  this->_leftUltrasonicTrig = trigLeft;
  this->_leftUltrasonicEcho = echoLeft;
}

void BBot::IRs(){
  if(this->mode != LineFollowing){ return; }

  int leftIR = digitalRead(this->_IR1);
  int middleIR = digitalRead(this->_IR2);
  int rightIR = digitalRead(this->_IR3);

  if( leftIR && !middleIR && rightIR ){ //forward
    this->_error = 0;
  }else if(leftIR && middleIR && !rightIR){ //Left
    this->_error = -1;
  }else if(leftIR && !middleIR && !rightIR){ //SharpLeft
    this->_error = -2;
  }else if(!leftIR && middleIR && rightIR){ //Right
    this->_error = 1;
  }else if(!leftIR && !middleIR && rightIR){ //SharpRight
    this->_error = 2;
  }
}

void BBot::calculatePID(){
  if(this->mode != LineFollowing){ return; }

  this->_p = this->_error;
  this->_d = this->_error - this->_previousError;
  this->_pdValue = (this->_kP * this->_p) + (this->_kD * this->_d);
  this->_previousError = this->_error;

  this->teleoperation(20, -this->_pdValue);
}

void BBot::RFID(){
  if(!this->isActive){ return; }
  if (!this->_rfid.PICC_IsNewCardPresent() || !this->_rfid.PICC_ReadCardSerial()) { return; }

  int cardId = this->_rfid.uid.uidByte[0] + this->_rfid.uid.uidByte[1] + this->_rfid.uid.uidByte[2] + this->_rfid.uid.uidByte[3];
  this->_rfid.PICC_HaltA();
  this->_rfid.PCD_StopCrypto1();

  if(this->goalMode == RFID1 && this->mode == LineFollowing){ this->RFID1GameHandler(cardId); }
  else if(this->goalMode == RFID2){ this->RFID2GameHandler(cardId); }
  else if(this->goalMode == RFIDProgramming){ this->RFIDProgrammingGameHandler(cardId); }
  else if(this->goalMode == Loop){ this->loopGameHandler(cardId); }
  else if(this->goalMode == Logic){ this->logicDesignerGameHandler(cardId); }
}

void BBot::RFID1GameHandler(int cardId){
  this->mode = RFID1;
  if(cardId == 514){
    Serial.print("F");
    this->isActive = false;
    this->mode = LineFollowing;
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
}

void BBot::RFID2GameHandler(int cardId){
  if(this->mode == LineFollowing && this->doneSetup == false){ //storing: CW:100 -> current && !active
    this->isActive = false;
    this->currentCardId = cardId;
    Serial.print("CW:"+(String)cardId);
  }else if(this->mode == LineFollowing && this->doneSetup == true){ //run as RFID1
    //add to the list
    this->mode = RFID2;
    if(this->cards[cardId] == "S"){
      Serial.print("F");
      this->isActive = false;
    }else if(this->cards[cardId] == "R"){
      this->action = Right;
      Serial.print("C1");
    }else if(this->cards[cardId] == "L"){
      this->action = Left;
      Serial.print("C1");
    }else if( cardId == 763){
      //this->action = SpeedUp;
      Serial.print("C1");
    }else if( cardId == 271){
      //this->action = SlowDown;
      Serial.print("C1");
    }
  }
}

void BBot::RFIDProgrammingGameHandler(int cardId){
  if(cardId == 487){
    this->isActive = false;
  }
  Serial.print(cardId);
}

void BBot::loopGameHandler(int cardId){
  if(cardId == 342){
    this->numberRounds++;
  }
  if(this->numberRounds == this->iterations){
    this->isActive = false;
    Serial.print("F");
  }else{
    Serial.print("C1");
  }
}

void BBot::logicDesignerGameHandler(int cardId){
  if(cardId == 422)
    this->logicalCards["A"] = 1;
  else if(cardId == 342) // cardId for card B
    this->logicalCards["B"] = 1;
    /* -------------------------- */
  if(this->and_or == "A"){
    if( (this->logicalCards["A"] == 1) && (this->logicalCards["B"] == 1) ){
      this->isActive = false;
      Serial.print("F");
    }
  }else if(this->and_or == "O"){
    if( (this->logicalCards["A"] == 1) || (this->logicalCards["B"] == 1) ){
      this->isActive = false;
      Serial.print("F");
    }
  }
}

void BBot::linkCurrentCardWithAction(String cardAction){
  this->isActive = true;
  this->cards[this->currentCardId] = cardAction;
  if(cardAction == "S"){
    this->doneSetup = true;
    this->isActive = false;
  }
}

void BBot::teleoperation(int xAxis, int yAxis){
  this->_linearVelocity =  constrain(xAxis, constants.xAngleMin, constants.xAngleMax);
  this->_linearVelocity = map(this->_linearVelocity, constants.xAngleMin, constants.xAngleMax, constants.velocityMin, constants.velocityMax);
  this->_angularVelocity = constrain(yAxis, constants.yAngleMin, constants.yAngleMax);
  this->_angularVelocity = map(this->_angularVelocity, constants.yAngleMin, constants.yAngleMax, constants.omegaMin, constants.omegaMax);
}

void BBot::move(){
  if(this->isActive){
    this->RFID1andRFID2MovementHandler();
  }else{
    this->teleoperation(0,0);
  }

  int velocityRightMotor = constrain((this->_linearVelocity - this->_angularVelocity), -255, 255);
  int velocityLeftMotor = constrain((this->_linearVelocity + this->_angularVelocity), -255, 255);

  if(velocityRightMotor < 0){
      analogWrite(this->_motorA1, velocityRightMotor * -1);
      analogWrite(this->_motorA2,0);
  }else{
      analogWrite(this->_motorA1,0);
      analogWrite(this->_motorA2, velocityRightMotor);
  }

  if(velocityLeftMotor < 0){
      analogWrite(this->_motorB1, velocityLeftMotor * -1);
      analogWrite(this->_motorB2,0);
  }else{
      analogWrite(this->_motorB1,0);
      analogWrite(this->_motorB2, velocityLeftMotor);
  }

  delay(10);
}

void BBot::RFID1andRFID2MovementHandler(){
  if(this->mode == RFID1 || this->mode == RFID2){
    switch (this->action) {
      case Right:
      this->teleoperation(0,-30);
      if((!dr(this->_IR1) && !dr(this->_IR2) && dr(this->_IR3)) || (!dr(this->_IR1) && dr(this->_IR2) && dr(this->_IR3))){
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
}

float BBot::distanceFromUltrasonic(Ultrasonics ultrasonics){
  int echo, trig;
  if(ultrasonics == BackUltrasonic){
      trig = this->_backUltrasonicTrig;
      echo = this->_backUltrasonicEcho;
    }else if(ultrasonics == RightUltrasonic){
      trig = this->_rightUltrasonicTrig;
      echo = this->_rightUltrasonicEcho;
    }else if(ultrasonics == LeftUltrasonic){
      trig = this->_leftUltrasonicTrig;
      echo = this->_leftUltrasonicEcho;
    }

    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(7);
    digitalWrite(trig,LOW);
    float time = pulseIn(echo,HIGH);
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
  //need to be updated later
  this->isActive = false;
}

void BBot::performActionWithSerial(String str){
  String operation = this->getValueFromString(str, ':', 0);
  String action = operation.substring(0);
  if(action == "G"){ //goal mode
    String mode = this->getValueFromString(str, ':', 1);
    if(mode == "1"){
      this->goalMode = Teleoperation;
      this->mode = Teleoperation;
    }else if(mode == "2"){
      this->goalMode = RFID1;
      this->mode = LineFollowing;
    }else if(mode == "3"){
      this->goalMode = RFID2;
      this->mode = LineFollowing;
    }else if(mode == "4"){
      this->goalMode = RFIDProgramming;
      this->mode = LineFollowing;
    }else if(mode == "5"){
      this->goalMode = Logic;
      this->mode = LineFollowing;
    }else if(mode == "6"){
      this->goalMode = Loop;
      this->mode = LineFollowing;
    }else if(mode == "7"){
      this->goalMode = ObstacleAvoidance;
      this->mode = Teleoperation;
    }
  }else if(action == "S"  || action == "SS"){ //start
    if (action == "SS") this->resetEveryThing();
    this->isActive = true;
  }else if(action == "P"){ //pause
    this->isActive = false;
  }else if(action == "V"){
    this->teleoperation(this->getValueFromString(str, ':', 1).toInt(), this->getValueFromString(str, ':', 2).toInt());
  }else if(action == "T"){
    this->mode = Teleoperation;
  }else if(action == "A"){
    this->mode = LineFollowing;
  }else if(action == "FS"){
    this->isActive = false;
  }else if(action == "L"){
    String cardAction = this->getValueFromString(str, ':', 1);
    this->linkCurrentCardWithAction(cardAction);
  }else if(action == "I"){
    this->iterations = this->getValueFromString(str, ':', 1).toInt();
    this->numberRounds = 0;
  }else if(action == "O"){
    this->logicalCards["A"] = 0;
    this->logicalCards["B"] = 0;
    this->and_or = this->getValueFromString(str, ':', 1);
  }
}

void BBot::obstacleAvoidanceHandler(){
  if(this->goalMode != ObstacleAvoidance) { return; }

  int distanceFromLeftUltrasonic = this->distanceFromUltrasonic(LeftUltrasonic);
  int distanceFromRightUltrasonic = this->distanceFromUltrasonic(RightUltrasonic);

  if(distanceFromRightUltrasonic < 30 && distanceFromLeftUltrasonic < 30){
    this->teleoperation(-30, 0);
  }else if(distanceFromLeftUltrasonic < 30){
    this->teleoperation(-30, 20);
  }else if(distanceFromRightUltrasonic < 30){
    this->teleoperation(-30, -20);
  }
}

void BBot::prepareForMovement(){
  this->RFID();
  this->IRs();
  this->calculatePID();
  //this->obstacleAvoidanceHandler();
}

void BBot::timerCallback(){

}
