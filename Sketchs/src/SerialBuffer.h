/*
  SerialBuffer.h - Reads a buffer from the serial port.

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
#ifndef SerialBuffer_h
#define SerialBuffer_h
#include "Arduino.h"

// Must be a multiple of 4. 0x1C (28) bytes is the maximum allowed since the
// Arduino buffer size is 64 bytes and to send a byte in ASCII in HEX we need
// two bytes. TIbeeProgrammer only allowed 0x10 as maximum since it's not
// implemented to read a block of bytes from differents banks.
#define BUFFER_LENGTH 0x10

class SerialBuffer {
public:
    SerialBuffer();

    void parse();

    byte readByte();
    unsigned short readBytes(byte buffer[]);
    unsigned long readUInt32();
    unsigned short readUInt16();
    void readString(char buffer[], int len);

    bool dataAvailable();
    unsigned short nextData(unsigned long* addr, byte buffer[]);

private:
    int numEntries;
    int currEntry;

    byte char2int(char ch);
};
#endif
