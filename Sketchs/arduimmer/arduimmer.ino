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
  digitalWrite(pinPGM, LOW);
  digitalWrite(pinVPP, HIGH);
  
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
   else if (strcmp(command, "Cnf!") == 0)
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
  int  bufferLength = 0;
  unsigned long address = 0;
  unsigned long tmpAddr;
  boolean contReceiving;
  boolean contProgramming = true;
  
  do {
    
    // First receive data from PC
    contReceiving = true;
    do {
      byte bytesRead;
      
      // Reads address (stop command -> address 0xFFFFFFFF)
      tmpAddr = serialReceiveAddress(); 
      if (address == 0 && tmpAddr != 0xFFFFFFFFuL)
        address = tmpAddr;
      
      // Set the write buffer length
      if (address < BOOT_BLOCK_LENGTH)
        bufferLength = BOOT_BLOCK_BUFFER;
      else if ((address & 0xFF0000) == 0x000000)
        bufferLength = CODE_BLOCK_BUFFER;
      else if ((address & 0xFF0000) == 0x3C0000)
        bufferLength = CONF_BLOCK_BUFFER;
      else if ((address & 0xFF0000) == 0x200000)
        bufferLength = IDLO_BLOCK_BUFFER;
      
      // If no "stop command", reads data
      if (tmpAddr != 0xFFFFFFFFuL) {
        bytesRead = serialReceiveData(buffer, bufferIdx);
        bufferIdx += bytesRead;
      }
      
      // If the Write Buffer is full
      if (bufferIdx >= bufferLength)
        contReceiving = false;
        
      // If the PC tells you "There is not more data to write" (stop command)
      if (tmpAddr == 0xFFFFFFFFuL) {
        contReceiving   = false;
        contProgramming = false;
      }
    } while (contReceiving);
    
    // Write buffer into memory
    // Loop specially useful when write buffer is 8 bytes
    // because we are receiving 16 bytes but only writing 8 bytes
    do {
      if (bufferIdx <= 0)
        break;
                
      enterLowVoltageIcsp();
      
      // Writes data
      int bytesToWrite = (bufferLength < bufferIdx) ? bufferLength : bufferIdx;
      progCodeMemory(address, buffer, bytesToWrite);
      
      // Reads written data to verify it
      byte b;
      for (int i = 0; i < bytesToWrite; i++) {
        // If the first iteration, writes the address too
        if (i == 0)
          b = readIncrMemory(address);
        else
          b = readIncrMemory();
          
        // Checks if they are the same. If not, shows error and quit.
        if (b != buffer[i]) {
          Serial.print("\t* ERROR checking at: 0x"); Serial.print(address + i, HEX);
          Serial.print(" | Tried: ");                Serial.print(buffer[i], HEX);
          Serial.print(" | Read: ");                 Serial.println(b, HEX);
          return;
        }
      }
      
      exitLowVoltageIcsp();
      
      // Show how good is going everything :D
      Serial.print("\t* |");
      if (bytesToWrite < 10)
        Serial.print("0");
      Serial.print(bytesToWrite);
      Serial.println(" bytes written correctly");
      
      // Copy not written data to the first positions (update buffer)
      if (bufferIdx >= bufferLength) {
        for (int i = 0; i < bufferIdx - bufferLength; i++)
          buffer[i] = buffer[bufferLength + i];
        
        bufferIdx -= bufferLength;
        address   += bufferLength;
      } else {
        bufferIdx = 0;
        address   = 0;
      }
    
    } while (bufferIdx >= bufferLength);
    
    // And repeats again until PC sends a null address
  } while (contProgramming);
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
