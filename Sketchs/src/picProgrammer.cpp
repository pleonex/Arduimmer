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
#include "Arduino.h"
#include "picProgrammer.h"

PicProgrammer::PicProgrammer(int dataPin, int clockPin, int masterPin, int vppPin)
                 : IcspProgrammer(dataPin, clockPin, false)
{
  this->masterPin = masterPin;
  this->vppPin = vppPin;
  init();
}

void PicProgrammer::init()
{
  IcspProgrammer::init();
  pinMode(masterPin, OUTPUT);
  pinMode(vppPin, OUTPUT);
}

boolean PicProgrammer::canRead()
{
  return true;
}

boolean PicProgrammer::canWrite()
{
  return true;
}

boolean PicProgrammer::canErase()
{
  return true;
}

boolean PicProgrammer::canShowDeviceId()
{
  return true;
}

void PicProgrammer::enterProgrammingMode()
{
  // 1º Set PGC & PGD low
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);

  // 2º Set PGM high
  digitalWrite(masterPin, HIGH);

  // 3º Wait at least 2 us
  delayMicroseconds(2);

  // 4º Set VPP high
  digitalWrite(vppPin, HIGH);

  // 5º Wait at least 2 us
  delayMicroseconds(2);
}

void PicProgrammer::exitProgrammingMode() {
  // 1º Set PGC & PGD low
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);

  // 2º Set VPP low
  digitalWrite(vppPin, LOW);

  // 3º Set PGM low
  digitalWrite(masterPin, LOW);
}

byte PicProgrammer::receiveByte() {

  // Set PGD low and wait 8 clock cycles
  digitalWrite(dataPin, LOW);
  for (byte i = 0; i < 8; i++) {
    digitalWrite(clockPin, HIGH);
    digitalWrite(clockPin, LOW);
  }

  // Read byte from PGD
  pinMode(dataPin, INPUT);
  byte data = 0;
  for (byte i = 0; i < 8; i++) {
    digitalWrite(clockPin, HIGH);
    bitWrite(data, i, digitalRead(dataPin));
    digitalWrite(clockPin, LOW);
  }

  // Set again PGD as output
  pinMode(dataPin, OUTPUT);

  return data;
}

byte PicProgrammer::sendInstruction(byte instr, unsigned int payload) {
  if (instr >= 2 && instr <= 0xB) {
    sendBits(instr, 4);
    return receiveByte();
  } else {
    sendBits(instr, 4);
    sendBits(payload, 16);
  }
}

void PicProgrammer::setTblPtr(unsigned long addr) {
  sendInstruction(InstCore, 0x0E00 | ((addr >> 16) & 0xFF)); // MOVLW 0x3F
  sendInstruction(InstCore, 0x6EF8);        // MOVWF TBLPTRU
  sendInstruction(InstCore, 0x0E00 | ((addr >>  8) & 0xFF)); // MOVLW 0xFF
  sendInstruction(InstCore, 0x6EF7);        // MOVWF TBLPTRH
  sendInstruction(InstCore, 0x0E00 | ((addr >>  0) & 0xFF)); // MOVLW 0xFE
  sendInstruction(InstCore, 0x6EF6);        // MOVWF TBLPTRL
}

/*---------------------------------------------------------------*/
/*                       Read functions                          */
/*---------------------------------------------------------------*/
void PicProgrammer::readBytes(unsigned long addr, byte buf[], int bufLen) {
  // 1º Set address into TBLPTR
  setTblPtr(addr);

  // 2º Read with Post-Increment
  buf[0] = sendInstruction(InstTblReadPostIncr, 0);

  // 3º Read as many bytes as needed
  for (int i = 1; i < bufLen; i++)
    buf[i] = readNextByte();
}

byte PicProgrammer::readNextByte() {
  return sendInstruction(InstTblReadPostIncr, 0);
}

/*---------------------------------------------------------------*/
/*                      Write functions                          */
/*---------------------------------------------------------------*/
void PicProgrammer::writeBytes(unsigned long addr, byte buf[], int bufLen) {
  // Check the maximum buffer size, and if lower just write
  int maxBufLen = getMaxBufferLength(addr);
  if (bufLen < maxBufLen) {
    writeBlock(addr, buf, bufLen);
    return;
  }

  // Let's split the buffer
  byte block[MAX_BLOCK_BUFFER];
  for (int start = 0; start < bufLen; start += maxBufLen) {
    // Block length
    int blockLen = (start + maxBufLen < bufLen) ? maxBufLen : bufLen - start;

    // Copy block of data
    for (int i = 0; i < blockLen; i++)
      block[i] = buf[start + i];

    // Write block
    writeBlock(addr + start, block, blockLen);
  }
}

int PicProgrammer::getMaxBufferLength(unsigned long address) {
  int bufferLength = -1;
  if (address < BOOT_BLOCK_LENGTH)
    bufferLength = BOOT_BLOCK_BUFFER;
  else if ((address & 0xFF0000) == 0x000000)
    bufferLength = CODE_BLOCK_BUFFER;
  else if ((address & 0xFF0000) == 0x300000)
    bufferLength = CONF_BLOCK_BUFFER;
  else if ((address & 0xFF0000) == 0x200000)
    bufferLength = IDLO_BLOCK_BUFFER;

  return bufferLength;
}

void PicProgrammer::writeBlock(unsigned long addr, byte buf[], int bufLen) {
  // Configure Device for Writes
  sendInstruction(InstCore, 0x8EA6);  // BSF  EECON1, EEPGD
  if (bufLen > 1)
    sendInstruction(InstCore, 0x9CA6);  // BCF  EECON1, CFGS
  else
    sendInstruction(InstCore, 0x8CA6);  // BSF  EECON1, CFGS

  // Set address
  setTblPtr(addr);

  // Load data into buffer
  int i;
  for (i = 0; i < (bufLen - 2); i += 2) {
    unsigned int data = (buf[i+1] << 8) | buf[i];
    sendInstruction(InstTblWritePostIncr, data);
  }

  // Write last two bytes and start programming
  unsigned int data = 0;
  if (bufLen > 1)
    data = (buf[i+1] << 8) | buf[i];
  else if (addr % 2 == 0)
    data = buf[i];
  else
    data = buf[i] << 8;

  sendInstruction(InstTblWriteProg, data);

  sendBits(0, 3);
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, HIGH);
  delayMicroseconds(TIME_P9);
  digitalWrite(clockPin, LOW);
  delayMicroseconds(TIME_P10);
  sendBits(0, 16);
}

void PicProgrammer::writeDirect(unsigned long addr, unsigned int data) {
  setTblPtr(addr);
  sendInstruction(InstTblWrite, data);
}

/*---------------------------------------------------------------*/
/*                      Erase functions                          */
/*---------------------------------------------------------------*/
bool PicProgrammer::erase() {
  const unsigned int mode = 0x3F8Fu; // CHIP ERASE

  // 1º Write mode into register
  writeDirect(0x3C0005, (highByte(mode) << 8) | highByte(mode));
  writeDirect(0x3C0004, (lowByte(mode) << 8) | lowByte(mode));

  // 2º Start erasing
    // 2.1 Send NOP
  sendInstruction(InstCore, 0);
    // 2.2 Send four '0' bits
  sendBits(0, 4);
    // 2.3 Wait P11 + P10 time
  digitalWrite(dataPin, LOW);
  digitalWrite(clockPin, LOW);
  delayMicroseconds(TIME_P11);
  delayMicroseconds(TIME_P10);
    // 2.4 Send null payload
  sendBits(0, 16);

  return true;
}

/*---------------------------------------------------------------*/
/*                   Chip Id functions                           */
/*---------------------------------------------------------------*/
unsigned int PicProgrammer::getDeviceId() {
  return readUInt16(0x3FFFFEL);
}

bool PicProgrammer::isSupported(unsigned int deviceId) {
  // If you want to test & support new device, add you PIC chip ID here
  if (deviceId & 0xFFE0 == 0x1080)
    return true;

  return false;
}
