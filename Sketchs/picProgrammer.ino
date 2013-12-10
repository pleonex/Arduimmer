/*
    Arduimmer. Using Arduino as a PIC18F2XXX and PIC18F4XXX programmer
    Copyright (C) 2013 Benito Palacios (benito356@gmail.com)
  
    This file is part of Arduimmer.

    Arduimmer is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Arduimmer is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Arduimmer.  If not, see <http://www.gnu.org/licenses/>. 
*/
void picProgrammerSetup() {
  pinMode(pinPGM, OUTPUT);
  pinMode(pinPGC, OUTPUT);
  pinMode(pinPGD, OUTPUT);
  pinMode(pinVPP, OUTPUT); 
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

void sendBit(byte data) {
  digitalWrite(pinPGC, HIGH);
  digitalWrite(pinPGD, data);
  
  digitalWrite(pinPGC, LOW);
  digitalWrite(pinPGD, LOW);
}

void sendBits(unsigned int data, int n) {
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
    bitWrite(data, i, digitalRead(pinPGD));  
    digitalWrite(pinPGC, LOW); 
  }
  
  // Set again PGD as output
  pinMode(pinPGD, OUTPUT);
  
  return data;
}

byte sendInstruction(byte instr, unsigned int payload) {
  if (bitRead(instr, 3) == 1 || instr == InstShiftOut) {
    sendBits(instr, 4);
    return receiveByte();
  } else {
    sendBits(instr, 4);
    sendBits(payload, 16);
  }
}

void setTblPtr(long addr) {
  sendInstruction(InstCore, 0x0E00 | ((addr >> 16) & 0xFF)); // MOVLW 0x3F
  sendInstruction(InstCore, 0x6EF8);        // MOVWF TBLPTRU
  sendInstruction(InstCore, 0x0E00 | ((addr >>  8) & 0xFF)); // MOVLW 0xFF
  sendInstruction(InstCore, 0x6EF7);        // MOVWF TBLPTRH
  sendInstruction(InstCore, 0x0E00 | ((addr >>  0) & 0xFF)); // MOVLW 0xFE
  sendInstruction(InstCore, 0x6EF6);        // MOVWF TBLPTRL
}

/*---------------------------------------------------------------*/
/*                       Read functions                          */
/*---------------------------------------------------------------*/
byte readIncrMemory(long addr) {
  // 1º Set address into TBLPTR
  setTblPtr(addr);
  
  // 2º Read with Post-Increment
  return sendInstruction(InstTblReadPostIncr, 0);
}

byte readIncrMemory() {
  return sendInstruction(InstTblReadPostIncr, 0); 
}

byte readMemory(long addr) {
  // 1º Set address into TBLPTR
  setTblPtr(addr);
  
  // 2º Read
  return sendInstruction(InstTblRead, 0);
}

byte readMemory() {
 return sendInstruction(InstTblRead, 0); 
}

/*---------------------------------------------------------------*/
/*                      Write functions                          */
/*---------------------------------------------------------------*/
void writeMemory(long addr, unsigned int data) {
  setTblPtr(addr);
  sendInstruction(InstTblWrite, data);
}

void writeMemory(unsigned int data) {
  sendInstruction(InstTblWrite, data); 
}

void writeIncrMemory(long addr, unsigned int data) {
  setTblPtr(addr);
  sendInstruction(InstTblWritePostIncr, data);
}

void writeIncrMemory(unsigned int data) {
  sendInstruction(InstTblWritePostIncr, data); 
}

void writeIncrProgMemory(long addr, unsigned int data) {
  setTblPtr(addr);
  sendInstruction(InstTblWritePostIncrProg, data);
}

void writeIncrProgMemory(unsigned int data) {
  sendInstruction(InstTblWritePostIncrProg, data); 
}

void writeProgMemory(long addr, unsigned int data) {
  setTblPtr(addr);
  sendInstruction(InstTblWriteProg, data);
}

void writeProgMemory(unsigned int data) {
  sendInstruction(InstTblWriteProg, data); 
}

void progCodeMemory(long addr, byte buf[], int bufLen) {
  // Configure Device for Writes
  sendInstruction(InstCore, 0x8EA6);  // BSF  EECON1, EEPGD
  sendInstruction(InstCore, 0x9CA6);  // BCF  EECON1, CFGS
  
  // Set address
  setTblPtr(addr);
  
  // Load data into buffer
  int i;
  for (i = 0; i < (bufLen - 2); i += 2) {
    unsigned int data = (buf[i] << 8) | buf[i+1];
    writeIncrMemory(data);
  }
  
  // Write last two bytes and start programming
  unsigned int data = (buf[i] << 8) | buf[i+1];
  writeProgMemory(data);
  
  sendBits(0, 4);
  digitalWrite(pinPGC, HIGH);
  delayMicroseconds(TIME_P9);
  digitalWrite(pinPGC, LOW);
  delayMicroseconds(TIME_P10);
  sendBits(0, 16);
}

/*---------------------------------------------------------------*/
/*                      Erase functions                          */
/*---------------------------------------------------------------*/
void perfomBulkErase(unsigned int mode) {
  // 1º Write mode into register
  writeMemory(0x3C0005, (lowByte(mode) << 8) | lowByte(mode));
  writeMemory(0x3C0006, (highByte(mode) << 8) | highByte(mode));

  // 2º Start erasing
    // 2.1 Send NOP
  sendInstruction(InstCore, 0);
    // 2.2 Send four '0' bits
  sendBits(0, 4);
    // 2.3 Wait P11 + P10 time
  digitalWrite(pinPGD, LOW);
  digitalWrite(pinPGC, LOW);
  delayMicroseconds(TIME_P11);
  delayMicroseconds(TIME_P10);
    // 2.4 Send null payload
  sendBits(0, 16);
  
  Serial.println("Erase done");
}
