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

const byte CMD_DEBUG_INSTR_1 = B01010101;    // Run 1 instruction in CPU
const byte CMD_DEBUG_INSTR_2 = B01010110;    // Run 2 instructions in CPU
const byte CMD_DEBUG_INSTR_3 = B01010111;    // Run 3 instructions in CPU
const byte CMD_CHIP_ERASE    = B00010000;    // Erase the chip
const byte CMD_WR_CONFIG     = B00011001;    // Write debug configuration
const byte CMD_RD_CONFIG     = B00100000;    // Read debug configuration
const byte CMD_BURST_WRITE   = B10000000;    // Write data to the buffer


// DUP registers (XDATA space address)
const unsigned short DUP_DBGDATA   = 0x6260;  // Debug interface data buffer
const unsigned short DUP_FCTL      = 0x6270;  // Flash controller
const unsigned short DUP_FWDATA    = 0x6273;  // Flash controller data buffer
const unsigned short DUP_FADDRL    = 0x6271;  // Flash controller addr
const unsigned short DUP_FADDRH    = 0x6272;  // Flash controller addr
const unsigned short DUP_DMA1CFGL  = 0x70D2;  // Low byte, DMA config ch. 1
const unsigned short DUP_DMA1CFGH  = 0x70D3;  // High byte, DMA config ch. 1
const unsigned short DUP_DMA0CFGL  = 0x70D4;  // Low byte, DMA config ch. 0
const unsigned short DUP_DMA0CFGH  = 0x70D5;  // High byte, DMA config ch. 0
const unsigned short DUP_DMAARM    = 0x70D6;  // DMA arming register
const unsigned short DUP_MEMCTR    = 0x70C7;  // Flash bank xdata mapping
const unsigned short DUP_CLKCONSTA = 0x709E;  // Sys clock status
const unsigned short DUP_CLKCONCMD = 0x70C6;  // Sys clock configuration

// DMA channels used on DUP
const byte CH_DBG_TO_BUF0   = 0x01; // Channel 0
const byte CH_BUF0_TO_FLASH = 0x02; // Channel 1

// Start addresses on DUP (Increased buffer size improves performance)
const unsigned short ADDR_BUF0       = 0x0000; // Buffer (512 bytes)
const unsigned short ADDR_DMA_DESC_0 = 0x0200; // DMA descriptors (8 bytes)
const unsigned short ADDR_DMA_DESC_1 = ADDR_DMA_DESC_0 + 8;

class Programmer
{
    public:
    Programmer(int pins[]);

    void enterProgrammingMode();
    void exitProgrammingMode();

    unsigned int getDeviceId();
    bool isSupported(unsigned int deviceId);

    void readBytes(unsigned long addr, byte buf[], unsigned short bufLen);
    void writeBytes(unsigned long addr, byte buf[], unsigned short bufLen);

    bool erase();

  private:
    // # Variables #
    int dataPin;
    int clockPin;
    int resetPin;
    bool isMsb;
    bool enableDMA;

    void init();

    void sendBit(byte data);
    void sendBits(unsigned int data, int n);
    unsigned int receiveBits(int n);
    unsigned int sendInstruction(byte command, byte *inst, int n);

    void initDMA();
    void writeByteXDATA(unsigned short addr, byte value);
    void writeBlockXDATA(unsigned short addr, byte buf[], unsigned short bufLen);
    void burstBlock(byte* buf, unsigned short bufLen);
    byte readByteXDATA(unsigned short addr);
};
#endif
