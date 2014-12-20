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

#define CMD_DEBUG_INSTR_1  B01010001    // Run 1 instruction in CPU
#define CMD_DEBUG_INSTR_2  B01010010    // Run 2 instructions in CPU
#define CMD_DEBUG_INSTR_3  B01010011    // Run 3 instructions in CPU
#define CMD_CHIP_ERASE     B00010000    // Erase the chip
#define CMD_WR_CONFIG      B00011001    // Write debug configuration
#define CMD_RD_CONFIG      B00100000    // Read debug configuration

#define DUP_CLKCONSTA 0x709E  // Sys clock status
#define DUP_CLKCONCMD 0x70C6  // Sys clock configuration

class TIbeeProgrammer : public IcspProgrammer
{
    public:
    TIbeeProgrammer(int dataPin, int clockPin, int resetPin);

    virtual void enterProgrammingMode();
    virtual void exitProgrammingMode();

    virtual unsigned int getDeviceId();
    virtual bool isSupported(unsigned int deviceId);

    virtual bool erase();

  protected:
    virtual void init();

    virtual void readBlock(unsigned long addr, byte buf[], int bufLen);
    virtual void writeBlock(unsigned long addr, byte buf[], int bufLen);
    virtual int getMaxBufferLength(unsigned long address);

  private:
    int resetPin;
    bool enableDMA;

    unsigned int receiveBits(int n);
    unsigned int sendInstruction(byte command, byte *inst, int n);

    void initDMA();
    void writeByteXDATA(unsigned long addr, byte value);
    void writeBlockXDATA(unsigned long addr, byte buf[], int bufLen);
    byte readByteXDATA(unsigned long addr);
};
#endif
