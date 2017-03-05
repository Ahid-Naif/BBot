#ifndef global_h
#define global_h
//shortcut for Serial.print() >> now use p() only
#define p(str) Serial.print(str)
//shortcut for Serial.println() >> now use pln() only
#define pln(str) Serial.println(str)
#define dw(pin,state) digitalWrite(pin,state)
#define dr(pin) digitalRead(pin)
#define aw(pin,val) analogWrite(pin,val)

#endif
