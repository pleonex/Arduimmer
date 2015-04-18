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
#include "TIbeeProgrammer.h"
#include "SerialBuffer.h"

const int CMD_LENGTH = 4;
char command[CMD_LENGTH + 1];

/*---------------------------------------------------------------*/
/*                           Messages                            */
/*---------------------------------------------------------------*/
const char PING[] = "Hey!";
const char PONG[] = "Yes?";
const char CODE[] = "Pro!";
const char ACK[]  = "ACK!";
const char READY[]    = "Ready!";
const char FINISHED[] = "Done!";
const char ERROR_UNKNOWN_DEVICE[] = "E01";
const char ERROR_INVALID_DEVICE[] = "E02";
const char ERROR_DATA_MISMATCH[]  = "E03";
const char ERROR_ERASE[]          = "E04";

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
  if (strcmp(command, PING) == 0)
    ping();
  else if (strcmp(command, CODE) == 0)
    program();
}

/*---------------------------------------------------------------*/
/*                  Reply to a ping command                      */
/* The PC uses this method to autodetect the USB Arduino port    */
/*---------------------------------------------------------------*/
void ping() {
  Serial.println(PONG);
}

/*---------------------------------------------------------------*/
/*                      Program a device                         */
/* This method will ask any additional info like program code    */
/*---------------------------------------------------------------*/
void program() {
  // Communication from serial buffer
  SerialBuffer* serialBuffer = new SerialBuffer();

  // Get configuration from PC
  // .. Read device name
  char deviceName[10];
  serialBuffer->readString(deviceName, 10);

  // .. Get all the ports
  int ports[10];
  int numPorts = serialBuffer->readByte();
  for (int i = 0; i < numPorts; i++)
    ports[i] = serialBuffer->readByte();

  // Create the programmer
  TIbeeProgrammer* programmer = new TIbeeProgrammer(ports[0], ports[1], ports[2]);
  if (programmer == NULL) {
    Serial.println(ERROR_UNKNOWN_DEVICE);
    return;
  }

  // Enter debug mode
  programmer->enterProgrammingMode();
  bool error = false;

  // Get device ID and verify it
  unsigned int deviceId = programmer->getDeviceId();
  error = !programmer->isSupported(deviceId);
  if (error) {
    Serial.println(ERROR_INVALID_DEVICE);
    return;
  }

  // Erase the cip
  error = !programmer->erase();
  if (error) {
    Serial.println(ERROR_ERASE);
    return;
  }

  // Delay to wait erase operation to complete
  delay(100);

  // Get program code from PC
  Serial.println(READY);
  serialBuffer->parse();

  // Write and verify the program
  while (serialBuffer->dataAvailable() && !error) {
    error = !writeNextBlock(programmer, serialBuffer);
    if (error)
      Serial.println(ERROR_DATA_MISMATCH);
    else
      Serial.println(ACK);
  }

  // Exit programming mode
  programmer->exitProgrammingMode();

  // Release memory
  delete programmer;

  // Say goodbye!
  Serial.println(FINISHED);
}

/*
 * Create the device programmer
 */
IcspProgrammer* programmerFactory(char deviceName[], int ports[])
{
  IcspProgrammer* programmer = NULL;

  // Choose by device name
  if (strcmp(deviceName, "cc25") == 0) {
    programmer = new TIbeeProgrammer(ports[0], ports[1], ports[2]);
  }

  return programmer;
}

/**
 * Write the next data block into the device memory.
 */
bool writeNextBlock(IcspProgrammer* programmer, SerialBuffer* serialBuffer) {
  unsigned long address;
  byte bufferWrite[BUFFER_LENGTH];
  byte bufferRead[BUFFER_LENGTH];

  // Get data to write
  unsigned short bufferLength = serialBuffer->nextData(&address, bufferWrite);

  // Write it
  programmer->writeBytes(address, bufferWrite, bufferLength);

  // Read it again
  programmer->readBytes(address, bufferRead, bufferLength);

  // Verify it
  bool success = true;
  for (int i = 0; i < bufferLength; i++) {
    success = (bufferWrite[i] == bufferRead[i]);
    if (bufferWrite[i] != bufferRead[i]) {
      Serial.print(i);
      Serial.print("(0x");
      Serial.print(address + i, HEX);
      Serial.print("): ");
      Serial.print(bufferWrite[i], HEX);
      Serial.print(" -> ");
      Serial.println(bufferRead[i], HEX);
    }
  }

  return success;
}
