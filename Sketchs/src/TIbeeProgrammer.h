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
#define CMD_BURST_WRITE    B10000000    // Write data to the buffer


// DUP registers (XDATA space address)
#define DUP_DBGDATA   0x6260  // Debug interface data buffer
#define DUP_FCTL      0x6270  // Flash controller
#define DUP_FWDATA    0x6273  // Flash controller data buffer
#define DUP_FADDRL    0x6271  // Flash controller addr
#define DUP_FADDRH    0x6272  // Flash controller addr
#define DUP_DMA1CFGL  0x70D2  // Low byte, DMA config ch. 1
#define DUP_DMA1CFGH  0x70D3  // High byte, DMA config ch. 1
#define DUP_DMA0CFGL  0x70D4  // Low byte, DMA config ch. 0
#define DUP_DMA0CFGH  0x70D5  // High byte, DMA config ch. 0
#define DUP_DMAARM    0x70D6  // DMA arming register
#define DUP_CLKCONSTA 0x709E  // Sys clock status
#define DUP_CLKCONCMD 0x70C6  // Sys clock configuration

// DMA channels used on DUP
#define CH_DBG_TO_BUF0   0x01 // Channel 0
#define CH_BUF0_TO_FLASH 0x02 // Channel 1

// Start addresses on DUP (Increased buffer size improves performance)
#define ADDR_BUF0       0x0000 // Buffer (512 bytes)
#define ADDR_DMA_DESC_0 0x0200 // DMA descriptors (8 bytes)
#define ADDR_DMA_DESC_1 (ADDR_DMA_DESC_0 + 8)

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
    void burstBlock(byte* buf, int bufLen);
    byte readByteXDATA(unsigned long addr);
};
#endif
