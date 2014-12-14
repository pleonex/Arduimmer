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
* Show Chip ID and Version
*/
void TIbeeProgrammer::showDeviceId()
{
  enterProgrammingMode();

  // Send "get chip command"
  sendBits(B01101000, 8);

  // Receive output
  unsigned int data = receiveBits(16);
  Serial.print("CHIP ID: 0x");
  Serial.println(data >> 8, HEX);
  Serial.print("CHIP VERSION: 0x");
  Serial.println(data & 0xFF, HEX);

  exitProgrammingMode();
}

void TIbeeProgrammer::erase()
{
  // TODO
}



//send 1, 2 or 3 instruction
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



//Write data in the flash memmory.
void TIbeeProgrammer::writeMemory(unsigned long addr, byte buf[], int bufLen)
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





/*   IN CONSTRUCTION
void TIbeeProgrammer::writeMemory(unsigned long addr, byte buf[], int bufLen)
{

//1º WR_CONFIG Command
sendBits(B00011000, 8);
sendBits(B00100010, 8);




//2º DMA Configurations:  Channel 0.
//SRCADDR Command
sendInstruction2(B01100010, B01100000);      //Send 2 bytes (Specified by last 2 bits)

//DESTADDR Command
sendInstruction2(B00000000, B00000000);
sendBits(B01010010, 8);      //Send 2 bytes
sendBits(B00000000, 8);
sendBits(B00000000, 8);

//VLEN and LEN Commands
sendBits(B01010010, 8);      //Send 2 bytes
sendBits(B000 << byte(bufLen), 8);  //!!!!!!! Not well made!!!
sendBits((byte(bufLen)), 8);  //!!!!!!! Not well made!!!

//WORDSIZE, TMODE AND TRIG Commands
sendBits(B01010001, 8);      //Send 1 byte
sendBits(B0 << B00 << B11111, 8);


//SRCINC, DESTINC, IRQMASK, M8 AND PRIORITY Commands
sendBits(B01010001, 8);      //Send 1 byte
sendBits(B00 << B01 << B0 << B0 << B01, 8);

//End of the Channel 0 configuration DMA.




//2º DMA Configurations:  Channel 1.
//SRCADDR Command
sendBits(B01010010, 8);      //Send 2 bytes
sendBits(B00000000, 8);
sendBits(B00000000, 8);

//DESTADDR Command
sendBits(B01010010, 8);      //Send 2 bytes
sendBits(B01100010, 8);
sendBits(B01110011, 8);

//VLEN and LEN Commands
sendBits(B01010010, 8);      //Send 2 bytes
sendBits(B000 << byte(bufLen), 8);  //!!!!!!! Not well made!!!
sendBits((byte(bufLen)), 8);  //!!!!!!! Not well made!!!

//WORDSIZE, TMODE AND TRIG Commands
sendBits(B01010001, 8);      //Send 1 byte
sendBits(B0 << B10 << B10010, 8);


//SRCINC, DESTINC, IRQMASK, M8 AND PRIORITY Commands
sendBits(B01010001, 8);      //Send 1 byte
sendBits(B01 << B01 << B0 << B0 << B10, 8);
//End of the Channel 1 configuration DMA.



//3º Point DMA Controller to DMA Configurations
//Prueba

}*/

void TIbeeProgrammer::writeMemory(unsigned long addr, unsigned int data)
{
  // TODO
}

byte TIbeeProgrammer::readMemory(unsigned long addr)
{
  // TODO
  return 0xFF;
}

byte TIbeeProgrammer::readMemoryIncr()
{
  // TODO
  return 0xFF;
}
