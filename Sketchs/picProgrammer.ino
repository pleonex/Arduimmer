const int pinPGM = 5;
const int pinPGC = 6;
const int pinPGD = 7;
const int pinVPP = 2;

void enterLowVoltageIcsp() {
  // 1º Set PGC & PGD low
  digitalWrite(pinPGC, LOW);
  digitalWrite(pinPGD, LOW);
  
  // 2º Set PGM high
  digitalWrite(pinPGM, HIGH);
  
  // 3º Wait at least 2 us
  delayMicroseconds(2);
  
  // 4º Set VPP high
  digitalWrite(pinVPP, HIGH);
  
  // 5º Wait at least 2 us
  delayMicroseconds(2);
}

void exitLowVoltageIcsp() {
  // 1º Set PGC & PGD low
  digitalWrite(pinPGC, LOW);
  digitalWrite(pinPGD, LOW);
  
  // 2º Set VPP low
  digitalWrite(pinVPP, LOW);
  
  // 3º Set PGM low
  digitalWrite(pinPGM, LOW);
}
