/*
  TIbeeProgrammer.h - Using Arduino as a CC2530 (8051 processor) programmer.

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
#ifndef TIbeeProgrammer_h
#define TIbeeProgrammer_h
#include "Arduino.h"
#include "IcspProgrammer.h"

class TIbeeProgrammer : public IcspProgrammer
{
    public:
    TIbeeProgrammer(int dataPin, int clockPin, int resetPin);

    virtual boolean canRead();
    virtual boolean canWrite();
    virtual boolean canErase();
    virtual boolean canShowDeviceId();

    virtual void enterProgrammingMode();
    virtual void exitProgrammingMode();

    virtual void showDeviceId();
    virtual void erase();
    virtual void writeMemory(unsigned long addr, byte buf[], int bufLen);
    virtual void writeMemory(unsigned long addr, unsigned int data);
    virtual unsigned int readMemory(unsigned long addr, int bufLen){
    virtual byte readMemory(unsigned long addr);
    virtual byte readMemoryIncr();

  protected:
    virtual void init();

  private:
    int resetPin;

    unsigned int receiveBits(int n);

    unsigned int sendInstruction(byte command, byte *inst, int n);

};




#endif
//Debug instructions for 1, 2 or 3 for sending commands
#define SEND_DEBUG_INSTR_1  B01010001
#define SEND_DEBUG_INSTR_2  B01010010
#define SEND_DEBUG_INSTR_3  B01010011
