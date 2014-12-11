/*
  IcspProgrammer.h - Abstract class for ICSP Programming with Arduino.

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
#ifndef IcspProgrammer_h
#define IcspProgrammer_h
#include "Arduino.h"

class IcspProgrammer
{
  public:
    IcspProgrammer(int dataPin, int clockPin);
    
    virtual boolean canRead() = 0;
    virtual boolean canWrite() = 0;
    virtual boolean canErase() = 0;
    virtual boolean canShowDeviceId() = 0;
    
    virtual void enterProgrammingMode() = 0;
    virtual void exitProgrammingMode() = 0;
    
    virtual void showDeviceId() = 0;
    virtual void erase() = 0;
    virtual void writeMemory(unsigned long addr, byte buf[], int bufLen) = 0;
    virtual void writeMemory(unsigned long addr, unsigned int data) = 0;
    virtual byte readMemory(unsigned long addr) = 0;
    virtual byte readMemoryIncr() = 0;
    
  protected:
    virtual void init();
    
    void sendBit(byte data);
    void sendBits(unsigned int data, int n);
    
    int dataPin;
    int clockPin;
};

#endif
