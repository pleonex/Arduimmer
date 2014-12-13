/*
    arduimmer.cpp - Using Arduino as an ICSP programmer.

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
#include "IcspProgrammer.h"
#include "picProgrammer.h"
#include "TIbeeProgrammer.h"

const int CMD_LENGTH = 4;
char command[CMD_LENGTH + 1];

/*---------------------------------------------------------------*/
/*                            Main setup                         */
/* Just setup serial port                                        */
/*---------------------------------------------------------------*/
void setup() {
  Serial.begin(9600);
  while (!Serial) ;   // Leonardo compability
}

/*---------------------------------------------------------------*/
/*                             Main loop                         */
/* Just receive and process commands                             */
/*---------------------------------------------------------------*/
void loop() {
  serialCommands();
}

/**
 * Receive commands by serial port and process them.
 */
void serialCommands() {
  if (Serial.available() < CMD_LENGTH)
    return;

  // Get command
  Serial.readBytes(command, CMD_LENGTH);

  // Procress command
  if (strcmp(command, "Hey!") == 0)
    ping();
  else if (strcmp(command, "Pro!") == 0)
    program();
}

/*---------------------------------------------------------------*/
/*                  Reply to a ping command                      */
/* The PC uses this method to autodetect the USB Arduino port    */
/*---------------------------------------------------------------*/
void ping() {
  Serial.println("Yes?");
}

/*---------------------------------------------------------------*/
/*                      Program a device                         */
/* This method will ask any additional info like program code    */
/*---------------------------------------------------------------*/
void program() {
  // Get configuration from PC
  // .. Read device name
  char deviceName[10];
  serialReceiveString(deviceName, 10);

  // .. Get all the ports
  int ports[10];
  int numPorts = Serial.read();
  for (int i = 0; i < numPorts; i++)
    ports[i] = Serial.read();

  // Get program code from PC
  // TODO

  // Create the programmer
  IcspProgrammer* programmer = programmerFactory(deviceName, ports);
  if (programmer == NULL) {
    Serial.println("Unknown device");
    return;
  }

  // Enter debug mode
  programmer->enterProgrammingMode();

  // Get device ID and verify it
  programmer->showDeviceId();
  // TODO: Verify it

  // Write code
  // TODO: writeCode(programmer);

  // Exit programming mode
  programmer->exitProgrammingMode();

  // Say goodbye!
  Serial.println("IEEE");
}

/*
 * Create the device programmer
 */
IcspProgrammer* programmerFactory(char deviceName[], int ports[])
{
  if (strcmp(deviceName, "pic18f") == 0){
    Serial.println("Selected PIC18F2xxx / PIC18F4xxx programmer");
    return new PicProgrammer(ports[0], ports[1], ports[2], ports[3]);
  } else if (strcmp(deviceName, "cc25") == 0) {
    Serial.println("Selected CC25xx programmer");
    return new TIbeeProgrammer(ports[0], ports[1], ports[2]);
  }

  return NULL;
}

void serialReceiveString(char buffer[], int len) {
  // Wait for all the data
  while (Serial.available() < len) ;
  Serial.readBytes(buffer, len);
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

void writeCode(IcspProgrammer* programmer) {
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

      // Writes data
      int bytesToWrite = (bufferLength < bufferIdx) ? bufferLength : bufferIdx;
      programmer->writeMemory(address, buffer, bytesToWrite);

      // Reads written data to verify it
      byte b;
      for (int i = 0; i < bytesToWrite; i++) {
        // If the first iteration, writes the address too
        if (i == 0)
          b = programmer->readMemory(address);
        else
          b = programmer->readMemoryIncr();

        // Checks if they are the same. If not, shows error and quit.
        if (b != buffer[i]) {
          Serial.print("\t* ERROR checking at: 0x"); Serial.print(address + i, HEX);
          Serial.print(" | Tried: ");                Serial.print(buffer[i], HEX);
          Serial.print(" | Read: ");                 Serial.println(b, HEX);
          return;
        }
      }

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

byte char2int(char ch) {
  if (ch >= '0' && ch <= '9')
   return ch - '0';
  if (ch >= 'A' && ch <= 'F')
   return 0xA + (ch - 'A');
  if (ch >= 'a' && ch <= 'f')
   return 0xA + (ch - 'a');

  return 0xFF;
}
