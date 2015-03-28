/*
  SerialBuffer.cpp - Reads a buffer from the serial port.

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
#include "SerialBuffer.h"

SerialBuffer::SerialBuffer() {
    currEntry = 0;
}

void SerialBuffer::parse() {
    numEntries = readUInt32();
}

bool SerialBuffer::dataAvailable() {
    return currEntry < numEntries;
}

int SerialBuffer::nextData(unsigned long* addr, byte buffer[]) {
    if (!dataAvailable())
        return 0;

    *addr  = readUInt32();
    int dataLength = readBytes(buffer);

    currEntry++;
    return dataLength;
}

byte SerialBuffer::readByte() {
    byte value = 0;

    // Wait for the data
    while (Serial.available() < 1*2) ;

    // Read a byte
    value = char2int(Serial.read()) << 4;
    value |= char2int(Serial.read());

    return value;
}

int SerialBuffer::readBytes(byte buffer[]) {
    int length = (int)readUInt32();

    // Wait for the data
    while (Serial.available() < length*2) ;

    // Read it
    for (int i = 0; i < length; i++)
        buffer[i] = readByte();

    return length;
}

unsigned long SerialBuffer::readUInt32() {
    unsigned long value = 0;

    // Wait for the data
    while (Serial.available() < 4*2) ;

    // Read 4 bytes
    for (byte i = 0; i < 4; i++)
        value |= (unsigned long)readByte() << (i * 8);

    return value;
}

/**
 * Receive a char arrary from serial port.
 */
void SerialBuffer::readString(char buffer[], int len) {
  // Wait for all the data
  while (Serial.available() < len) ;
  Serial.readBytes(buffer, len);
}

byte SerialBuffer::char2int(char ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'A' && ch <= 'F')
        return 0xA + (ch - 'A');
    if (ch >= 'a' && ch <= 'f')
        return 0xA + (ch - 'a');

    return 0xFF;
}
