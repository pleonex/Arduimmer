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
#include "SerialBuffer.h"

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
  SerialBuffer* serialBuffer = new SerialBuffer();
  serialBuffer->parse();

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

  // Write and verify the program
  while (serialBuffer->dataAvailable())
    writeBlockMemory(programmer, serialBuffer);

  // Exit programming mode
  programmer->exitProgrammingMode();

  // Say goodbye!
  Serial.println("Have a nice day :)");
}

/*
 * Create the device programmer
 */
IcspProgrammer* programmerFactory(char deviceName[], int ports[])
{
  char *fullName;
  IcspProgrammer* programmer = NULL;

  // Choose by device name
  if (strcmp(deviceName, "pic18f") == 0){
    fullName   = "PIC18F2xxx / PIC18F4xxx";
    programmer = new PicProgrammer(ports[0], ports[1], ports[2], ports[3]);
  } else if (strcmp(deviceName, "cc25") == 0) {
    fullName   = "CC25xx";
    programmer = new TIbeeProgrammer(ports[0], ports[1], ports[2]);
  }

  // Show selection
  Serial.print("Selected ");
  Serial.print(fullName == NULL ? deviceName : fullName);
  Serial.println(" programmer");

  return programmer;
}

/**
 * Receive a char arrary from serial port.
 */
void serialReceiveString(char buffer[], int len) {
  // Wait for all the data
  while (Serial.available() < len) ;
  Serial.readBytes(buffer, len);
}

/**
 * Write data into a block of memory.
 */
void writeBlockMemory(IcspProgrammer* programmer, SerialBuffer* serialBuffer) {
    unsigned long address;
    byte buffer[BUFFER_LENGTH];

    // Get data to write
    int bufferLength = serialBuffer->nextData(&address, buffer);

    // Write it
    programmer->writeMemory(address, buffer, bufferLength);

    // Read it again
    // TODO

    // Verify it
    // TODO

    // Show how good is going everything :D
    Serial.print("\t* |");
    if (bufferLength < 10)
      Serial.print("0");
    Serial.print(bufferLength);
    Serial.println(" bytes written correctly");
}
