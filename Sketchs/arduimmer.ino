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
#include "picProgrammer.h"

const int CMD_LENGTH = 4;
char command[CMD_LENGTH + 1];

void setup() { 
  picProgrammerSetup();
  
  Serial.begin(9600);
  while (!Serial) ;   // Leonardo compability
}

void loop() {
  serialCommands();
}

void serialCommands() {
  if (Serial.available() < CMD_LENGTH)
    return;
  
   Serial.readBytes(command, CMD_LENGTH);   
   if (strcmp(command, "Hey!") == 0)
     ping();
   else if (strcmp(command, "Dev?") == 0)
     showDeviceId();
   else if (strcmp(command, "Goo!") == 0)
     enterLowVoltageIcsp();
   else if (strcmp(command, "End!") == 0)
     exitLowVoltageIcsp();
   else if (strcmp(command, "Era!") == 0)
     perfomBulkErase(ERASE_CHIP);
   else if (strcmp(command, "Cod!") == 0)
     writeCode();
 
 // Clear buffer
 while (Serial.available() > 0)
   Serial.read();
}

void ping() {
  Serial.println("Yes?");
}

byte serialReceiveByte() {
  byte value = 0;
  
  while (Serial.available() < 2) ; 
  value = char2int(Serial.read()) << 4;
  value |= char2int(Serial.read());

  return value;
}

unsigned long serialReceiveAddress() {
  unsigned long address = 0;
  
  while (Serial.available() < 4*2) ;
  for (byte i = 0; i < 4; i++)
    address |= (unsigned long)serialReceiveByte() << (i * 8);
  
  return address;
}

byte serialReceiveData(byte buffer[], int idx) {
  byte bufferLength;
  
  while (Serial.available() < 1*2) ;
  bufferLength = serialReceiveByte();
  
  while (Serial.available() < bufferLength*2) ;
  for (int i = 0; i < bufferLength; i++)
    buffer[idx + i] = serialReceiveByte();
  
  return bufferLength;
}

void writeCode() {
  byte buffer[64 * 16];
  int  bufferIdx = 0;
  unsigned long address;
  unsigned long tmpAddr;
  boolean contReceiving;
  boolean contProgramming = true;
  
  do {
    // Reset variables
    contReceiving = true;
    if (bufferIdx >= WRITE_BUFFER) {
      // Copy not written data to the first positions
      for (int i = 0; i < bufferIdx - WRITE_BUFFER; i++)
        buffer[i] = buffer[WRITE_BUFFER + i];
      bufferIdx -= WRITE_BUFFER;
      address += WRITE_BUFFER;
    } else {
      address = 0;
      bufferIdx = 0;
    }
    
    // 1º Get address and code
    do {
      byte bytesRead;
      
      tmpAddr = serialReceiveAddress(); 
      if (address == 0)
        address = tmpAddr + BOOT_BLOCK;
        
      if (tmpAddr != 0xFFFFFFFFuL) {
        bytesRead = serialReceiveData(buffer, bufferIdx);
        bufferIdx += bytesRead;
      }
      
      // If the Write Buffer is full
      if (bufferIdx >= WRITE_BUFFER)
        contReceiving = false;
        
      // If the PC tell you "There is not more data to write atm"
      if (tmpAddr == 0xFFFFFFFFuL) {
        contReceiving   = false;
        contProgramming = false;
      }
      
      Serial.println("ACK");
    } while (contReceiving);
    
    if (bufferIdx > 0) {
      // DEBUG
      Serial.print("Address: 0x");        Serial.println(address, HEX);
      Serial.print("Buffer length: 0x");  Serial.println(bufferIdx, HEX);
      for (int i = 0; i < bufferIdx; i++) {
        if (buffer[i] < 0x10)
          Serial.print('0');
        Serial.print(buffer[i], HEX);
      }
      Serial.println("");
     
      // 3º Write code into memory
      // UNDONE
    }
  } while (contProgramming);
  
  Serial.println("Done!");
}

void showDeviceId() {
  enterLowVoltageIcsp();
  
  short deviceId = readIncrMemory(0x3FFFFEL);
  deviceId |= readIncrMemory() << 8;
  
  exitLowVoltageIcsp();
  
  Serial.println(deviceId, HEX);
}

byte char2int(char ch) {
  if (ch >= '0' && ch <= '9')
   return ch - '0';
  if (ch >= 'A' && ch <= 'F')
   return 0xA + (ch - 'A');
  if (ch >= 'a' && ch <= 'f')
   return 0xA + (ch - 'a');

  return 0xFF; 
}
