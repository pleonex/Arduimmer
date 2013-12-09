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
}

void ping() {
  Serial.println("Yes?");
}

void showDeviceId() {
  enterLowVoltageIcsp();
  
  // 1º Set 3FFFFEh address into TBLPTR
  sendInstruction(InstCore, 0x0E00 | 0x3F); // MOVLW 0x3F
  sendInstruction(InstCore, 0x6EF8);        // MOVWF TBLPTRU
  sendInstruction(InstCore, 0x0E00 | 0xFF); // MOVLW 0xFF
  sendInstruction(InstCore, 0x6EF7);        // MOVWF TBLPTRH
  sendInstruction(InstCore, 0x0E00 | 0xFE); // MOVLW 0xFE
  sendInstruction(InstCore, 0x6EF6);        // MOVWF TBLPTRL
  
  // 2º Read Id
  short deviceId = 0;
  deviceId = sendInstruction(InstTblReadPostIncr, 0);
  deviceId |= sendInstruction(InstTblReadPostIncr, 0) << 8;
  
  exitLowVoltageIcsp();
  
  Serial.print("Device ID: 0x");
  Serial.println(deviceId, HEX);
}




