/*
  PicProgrammer.h - Using Arduino as a PIC18F2XXX and PIC18F4XXX programmer.

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
#ifndef PicProgrammer_h
#define PicProgrammer_h
#include "Arduino.h"
#include "IcspProgrammer.h"

#define TIME_P9   1000
#define TIME_P10   100
#define TIME_P11  5000

#define BOOT_BLOCK_LENGTH  0x800
#define BOOT_BLOCK_BUFFER  8
#define CODE_BLOCK_BUFFER  32
#define CONF_BLOCK_BUFFER  1
#define IDLO_BLOCK_BUFFER  8

#define InstCore                 0x0
#define InstShiftOut             0x2
#define InstTblRead              0x8
#define InstTblReadPostIncr      0x9
#define InstTblReadPostDecr      0xA
#define InstTblReadPreIncr       0xB
#define InstTblWrite             0xC
#define InstTblWritePostIncr     0xD
#define InstTblWritePostIncrProg 0xE
#define InstTblWriteProg         0xF

class PicProgrammer : public IcspProgrammer
{
  public:
    PicProgrammer(int dataPin, int clockPin, int masterPin, int vppPin);

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
    int masterPin;
    int vppPin;

    byte receiveByte();
    byte sendInstruction(byte instr, unsigned int payload);
    void setTblPtr(unsigned long addr);

    byte readNextByte();
    void writeDirect(unsigned long addr, unsigned int data);
};

#endif
