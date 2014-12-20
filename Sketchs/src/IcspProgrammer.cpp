/*
  IcspProgrammer.cpp - Abstract class for ICSP Programming with Arduino.

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
#include "IcspProgrammer.h"

IcspProgrammer::IcspProgrammer(int dataPin, int clockPin, boolean isMsb)
{
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  this->isMsb = isMsb;
}

void IcspProgrammer::init()
{
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

/**
 * Send a bit to the processor.
 */
void IcspProgrammer::sendBit(byte data) {
  // The bit is transmitted in
  // while the clockPin is high.
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, data);

  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
}

/**
 * Send a bit stream to the processor.
 */
void IcspProgrammer::sendBits(unsigned int data, int n) {
  if (isMsb) {
    for (int i = n - 1; i >= 0; i--)
      sendBit(bitRead(data, i));
  } else {
    for (int i = 0; i < n; i++)
      sendBit(bitRead(data, i));
  }
}

/*---------------------------------------------------------------*/
/*                       Read functions                          */
/*---------------------------------------------------------------*/
void IcspProgrammer::readBytes(unsigned long addr, byte buf[], int bufLen) {
  // Check the maximum buffer size, and if lower just read
  int maxBufLen = getMaxBufferLength(addr);
  if (bufLen < maxBufLen) {
    readBlock(addr, buf, bufLen);
    return;
  }

  // Let's split the buffer
  byte block[BUFFER_LENGTH];
  for (int start = 0; start < bufLen; start += maxBufLen) {
    // Block length
    int blockLen = (start + maxBufLen < bufLen) ? maxBufLen : bufLen - start;

    // Read block
    readBlock(addr + start, block, blockLen);

    // Copy block of data
    for (int i = 0; i < blockLen; i++)
      buf[start + i] = block[i];
  }
}

/**
 * Read a byte from the memory.
 */
byte IcspProgrammer::readByte(unsigned long addr) {
  byte buffer[1];
  readBytes(addr, buffer, 1);
  return buffer[0];
}

/**
 * Read an integer value of 16 bits.
 */
unsigned short IcspProgrammer::readUInt16(unsigned long addr) {
  byte buffer[2];
  readBytes(addr, buffer, 2);

  if (isMsb) {
    return (buffer[0] << 8) | buffer[1];
  } else {
    return (buffer[1] << 8) | buffer[0];
  }
}

/**
 * Read an integer value of 32 bits.
 */
unsigned int IcspProgrammer::readUInt32(unsigned long addr) {
  byte buffer[4];
  readBytes(addr, buffer, 4);

  if (isMsb) {
    return (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
  } else {
    return (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
  }
}

/*---------------------------------------------------------------*/
/*                      Write functions                          */
/*---------------------------------------------------------------*/
void IcspProgrammer::writeBytes(unsigned long addr, byte buf[], int bufLen) {
  // Check the maximum buffer size, and if lower just write
  int maxBufLen = getMaxBufferLength(addr);
  if (bufLen < maxBufLen) {
    writeBlock(addr, buf, bufLen);
    return;
  }

  // Let's split the buffer
  byte block[BUFFER_LENGTH];
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

void IcspProgrammer::writeByte(unsigned long addr, byte data) {
  byte buffer[1] = { data };
  writeBytes(addr, buffer, 1);
}

void IcspProgrammer::writeUInt16(unsigned long addr, unsigned short data) {
  byte buffer[2];
  if (isMsb) {
    buffer[0] = data >> 8;
    buffer[1] = data & 0xFF;
  } else {
    buffer[1] = data >> 8;
    buffer[0] = data & 0xFF;
  }

  writeBytes(addr, buffer, 2);
}

void IcspProgrammer::writeUInt32(unsigned long addr, unsigned int data) {
  byte buffer[4];
  if (isMsb) {
    buffer[0] = (data >> 24) & 0xFF;
    buffer[1] = (data >> 16) & 0xFF;
    buffer[2] = (data >> 8) & 0xFF;
    buffer[3] = data & 0xFF;
  } else {
    buffer[3] = (data >> 24) & 0xFF;
    buffer[2] = (data >> 16) & 0xFF;
    buffer[1] = (data >> 8) & 0xFF;
    buffer[0] = data & 0xFF;
  }

  writeBytes(addr, buffer, 2);
}
