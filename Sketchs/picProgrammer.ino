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
    bitWrite(data, i, digitalRead(pinPGD));  
    digitalWrite(pinPGC, LOW); 
  }
  
  // Set again PGD as output
  pinMode(pinPGD, OUTPUT);
  
  return data;
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
