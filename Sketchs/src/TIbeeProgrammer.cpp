/*
TIbeeProgrammer.cpp - Using Arduino as a CC2530 (8051 processor) programmer.

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
#include "Arduino.h"
#include "TIbeeProgrammer.h"

TIbeeProgrammer::TIbeeProgrammer(int dataPin, int clockPin, int resetPin)
: IcspProgrammer(dataPin, clockPin, true)
{
  this->resetPin = resetPin;
  init();
}

/**
* Configure the pin to use.
*/
void TIbeeProgrammer::init()
{
  IcspProgrammer::init();
  pinMode(resetPin, OUTPUT);
}

boolean TIbeeProgrammer::canRead()
{
  return false;
}

boolean TIbeeProgrammer::canWrite()
{
  return false;
}

boolean TIbeeProgrammer::canErase()
{
  return false;
}

boolean TIbeeProgrammer::canShowDeviceId()
{
  return true;
}

/**
* Enter into Debug Mode.
*/
void TIbeeProgrammer::enterProgrammingMode()
{
  digitalWrite(resetPin, LOW);

  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  digitalWrite(resetPin, HIGH);
}

void TIbeeProgrammer::exitProgrammingMode()
{
  // This is a guess... no official docs
  digitalWrite(resetPin, LOW);
  digitalWrite(resetPin, HIGH);
}

/**
* Receive a bit stream from the processor.
*/
unsigned int TIbeeProgrammer::receiveBits(int n)
{
  // Wait for the chip to be ready
  // indicated by setting dataPin to high.
  pinMode(dataPin, INPUT);
  while(digitalRead(dataPin) == HIGH){
    // If chip is still not ready...
    // send 8 clockPin pulses.
    for (int i = 0; i < 8; i++) {
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
  }

  // Now, it's ready, start to receive dataPin...
  unsigned int data = 0;
  for (int i = n - 1; i >= 0; i--) {
    // Send a clockPin pulse to get next bit.
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
    bitWrite(data, i, digitalRead(dataPin));
  }

  // Set again dataPin as output (default)
  pinMode(dataPin, OUTPUT);

  return data;
}

/**
 * Send 1, 2 or 3 instructions
 */
unsigned int TIbeeProgrammer::sendInstruction(byte command, byte *inst, int n)
{
  //Send command
  sendBits(command, 8);

  //Send bytes
  for(int i = 0; i < n; i++){
    sendBits(inst[i], 8);
  }

  unsigned int output = receiveBits(8);

  return output;
}

/*---------------------------------------------------------------*/
/*                   Chip Id functions                           */
/*---------------------------------------------------------------*/
unsigned int TIbeeProgrammer::getDeviceId()
{
  // Send "get chip command"
  sendBits(B01101000, 8);

  // Receive output
  return receiveBits(16);
}

bool TIbeeProgrammer::isSupported(unsigned int deviceId)
{
    if (deviceId & 0xFF00 == 0xA500)
        return true;

    return false;
}

/*---------------------------------------------------------------*/
/*                      Erase functions                          */
/*---------------------------------------------------------------*/
bool TIbeeProgrammer::erase()
{
  // Send erase command and receive debug status
  sendBits(B00010000,8);
  byte status = receiveBits(8);

  // Check if erase has started
  byte isErasing = status >> 7;
  if (isErasing == 0) {
    return false;
  }

  // Wait until erase has finished or max iteration reached
  for(int i = 1; i <= 10; i++) {
    // Request debug status with "RD_CONFIG"
    sendBits(B00110000,8);
    status = receiveBits(8);

    // Check if ease has finished
    isErasing = status >> 7;
    if (isErasing == 0){
      return true;
    }
  }

  // Max iterations reached
  return false;
}

/*---------------------------------------------------------------*/
/*                      Write functions                          */
/*---------------------------------------------------------------*/
void TIbeeProgrammer::writeBlock(unsigned long addr, byte buf[], int bufLen)
{
  unsigned char instr[3];

  //MOV DPTR, Address
  instr[0] = 0x90;
  instr[1] = highByte(addr);
  instr[2] = lowByte(addr);

  sendInstruction(SEND_DEBUG_INSTR_3, instr, 3);

  for(int i = 0; i < bufLen; i++){
    //MOV A, values[i]
    instr[0] = 0x74;
    instr[1] = buf[i];
    sendInstruction(SEND_DEBUG_INSTR_2, instr, 2);

    //MOV A, values[i]
    instr[0] = 0xF0;
    sendInstruction(SEND_DEBUG_INSTR_1, instr, 1);

    // INC DPTR
    instr[0] = 0xA3;
    sendInstruction(SEND_DEBUG_INSTR_1, instr, 1);

  }
}

int TIbeeProgrammer::getMaxBufferLength(unsigned long address) {
  return 1024;
}

/*---------------------------------------------------------------*/
/*                       Read functions                          */
/*---------------------------------------------------------------*/
byte TIbeeProgrammer::readByte(unsigned long addr)
{
  unsigned char instr[3];

  // MOV DPTR, address
  instr[0] = 0x90;
  instr[1] = highByte(addr);
  instr[2] = lowByte(addr);
  sendInstruction(SEND_DEBUG_INSTR_3, instr, 3);

  // MOVX A, @DPTR
  instr[0] = 0xE0;
  return sendInstruction(SEND_DEBUG_INSTR_1, instr, 1);
}

void TIbeeProgrammer::readBytes(unsigned long addr, byte buffer[], int bufLen)
{
  for(int i = 0; i < bufLen; i++){
    buffer[i] = readByte(addr + i);
  }
}
