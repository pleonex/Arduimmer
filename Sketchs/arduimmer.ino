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




