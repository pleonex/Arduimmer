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

#define MAX_ENTRIES 5
#define BUFFER_LENGTH 2048

class SerialBuffer {
public:
    SerialBuffer();

    void parse();

    byte readByte();
    int readBytes(byte buffer[]);
    unsigned long readUInt32();
    void readString(char buffer[], int len);

    bool dataAvailable();
    int nextData(unsigned long* addr, byte buffer[]);

private:
    unsigned long address[MAX_ENTRIES];
    byte data[MAX_ENTRIES][BUFFER_LENGTH];
    int dataLength[MAX_ENTRIES];

    byte numEntries;
    byte currEntry;

    byte char2int(char ch);
};
#endif
