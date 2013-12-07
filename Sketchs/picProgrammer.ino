const int pinPGM = 5;
const int pinPGC = 6;
const int pinPGD = 7;
const int pinVPP = 2;

const byte InstCore                 = 0x0;
const byte InstShiftOut             = 0x2;
const byte InstTblRead              = 0x8;
const byte InstTblReadPostIncr      = 0x9;
const byte InstTblReadPostDecr      = 0xA;
const byte InstTblReadPreIncr       = 0xB;
const byte InstTblWrite             = 0xC;
const byte InstTblWritePostIncr     = 0xD;
const byte InstTblWritePostIncrProg = 0xE;
const byte InstTblWriteProg         = 0xF;

void picProgrammerSetup() {
  pinMode(pinPGM, OUTPUT);
  pinMode(pinPGC, OUTPUT);
  pinMode(pinPGD, OUTPUT);
  pinMode(pinVPP, OUTPUT); 
}

byte sendInstruction(byte instr, short payload) {
  if (bitRead(instr, 3) == 1 || instr == InstShiftOut) {
    sendBits(instr, 4);
    return receiveByte();
  } else {
    sendBits(instr, 4);
    sendBits(payload, 16);
  }
}

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

void sendBit(bit data) {
  digitalWrite(pinPGC, HIGH);
  digitalWrite(pinPGD, data);
  
  digitalWrite(pinPGC, LOW);
  digitalWrite(pinPGD, LOW);
}

void sendBits(short data, int n) {
  for (int i = 0; i < n; i++)
    sendBit(bitRead(data, i));
}

byte receiveByte() {
  // Set PGD low and wait 8 clock cycles
  digitalWrite(pinPGD, LOW);
  for (byte i = 0; i < 8; i++) {
    digitalWrite(pinPGC, HIGH);
    digitalWrite(pinPGC, LOW);
  }
  
  // Read byte from PGD
  pinMode(pinPGD, INPUT);
  byte data = 0;
  for (byte i = 0; i < 8; i++) {
    digitalWrite(pinPGC, HIGH);
    bitWrite(data, i, digitalRead(pinPGC));
    digitalWrite(pinPGC, LOW);
  }
  
  // Set again PGD as output
  pinMode(pinPGD, OUTPUT);
}
