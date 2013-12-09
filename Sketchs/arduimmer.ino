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
}

void ping() {
  Serial.println("Yes?");
}

void showDeviceId() {
  enterLowVoltageIcsp();
  
  short deviceId = readIncrMemory(0x3FFFFEL);
  deviceId |= readIncrMemory() << 8;
  
  exitLowVoltageIcsp();
  
  Serial.println(deviceId, HEX);
}


