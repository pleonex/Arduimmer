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

IcspProgrammer::IcspProgrammer(int dataPin, int clockPin)
{
  this->dataPin = dataPin;
  this->clockPin = clockPin;
  
  init();
}

void IcspProgrammer::init()
{
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
}

void IcspProgrammer::sendBit(byte data) {
  digitalWrite(clockPin, HIGH);
  digitalWrite(dataPin, data);
  
  digitalWrite(clockPin, LOW);
  digitalWrite(dataPin, LOW);
}

void IcspProgrammer::sendBits(unsigned int data, int n) {
  for (int i = 0; i < n; i++)
    sendBit(bitRead(data, i));
}
