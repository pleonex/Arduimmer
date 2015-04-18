/*
TIbeeProgrammer.cpp - Using Arduino as a CC2530 (8051 processor) programmer.

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
#include "TIbeeProgrammer.h"

TIbeeProgrammer::TIbeeProgrammer(int dataPin, int clockPin, int resetPin)
: IcspProgrammer(dataPin, clockPin, true)
{
  this->resetPin  = resetPin;
  this->enableDMA = false;
  init();
}

/**
* Configure the pin to use.
*/
void TIbeeProgrammer::init()
{
  IcspProgrammer::init();
  pinMode(resetPin, OUTPUT);
}

/**
* Enter into Debug Mode.
*/
void TIbeeProgrammer::enterProgrammingMode()
{
  digitalWrite(resetPin, LOW);

  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);
  digitalWrite(clockPin, HIGH);
  digitalWrite(clockPin, LOW);

  digitalWrite(resetPin, HIGH);
}

void TIbeeProgrammer::exitProgrammingMode()
{
  // This is a guess... no official docs
  digitalWrite(resetPin, LOW);
  digitalWrite(resetPin, HIGH);
}

/**
* Receive a bit stream from the processor.
*/
unsigned int TIbeeProgrammer::receiveBits(int n)
{
  // Wait for the chip to be ready
  // indicated by setting dataPin to high.
  pinMode(dataPin, INPUT);
  while(digitalRead(dataPin) == HIGH){
    // If chip is still not ready...
    // send 8 clockPin pulses.
    for (int i = 0; i < 8; i++) {
      digitalWrite(clockPin, HIGH);
      digitalWrite(clockPin, LOW);
    }
  }

  // Now, it's ready, start to receive dataPin...
  unsigned int data = 0;
  for (int i = n - 1; i >= 0; i--) {
    // Send a clockPin pulse to get next bit.
    digitalWrite(clockPin, HIGH);
    bitWrite(data, i, digitalRead(dataPin));
    digitalWrite(clockPin, LOW);
  }

  // Set again dataPin as output (default)
  pinMode(dataPin, OUTPUT);

  return data;
}

/**
 * Send 1, 2 or 3 instructions
 */
unsigned int TIbeeProgrammer::sendInstruction(byte command, byte inst[], int n)
{
  // Send command
  sendBits(command, 8);

  // Send bytes
  for(int i = 0; i < n; i++)
    sendBits(inst[i], 8);

  // Receive output byte
  return receiveBits(8);
}

void TIbeeProgrammer::initDMA() {
  // Enable DMA (Disable DMA_PAUSE bit in debug configuration)
  byte debug_config = 0x22;
  sendInstruction(CMD_WR_CONFIG, &debug_config, 1);
}

/*---------------------------------------------------------------*/
/*                   Chip Id functions                           */
/*---------------------------------------------------------------*/
unsigned int TIbeeProgrammer::getDeviceId()
{
  // Send "get chip command"
  sendBits(B01101000, 8);

  // Receive output
  return receiveBits(16);
}

bool TIbeeProgrammer::isSupported(unsigned int deviceId)
{
    if ((deviceId & 0xFF00) == 0xA500)
        return true;

    return false;
}

/*---------------------------------------------------------------*/
/*                      Erase functions                          */
/*---------------------------------------------------------------*/
bool TIbeeProgrammer::erase()
{
  // Send erase command and receive debug status
  byte status = sendInstruction(CMD_CHIP_ERASE, NULL, 0);

  // Check if erase has started
  byte isErasing = status >> 7;
  if (isErasing == 0)
    return false;

  // Wait until erase has finished or max iteration reached
  for(int i = 1; i <= 10; i++) {
    // Request debug status with "RD_CONFIG"
    status = sendInstruction(CMD_RD_CONFIG, NULL, 0);

    // Check if ease has finished
    isErasing = status >> 7;
    if (isErasing == 0)
      return true;
  }

  // Max iterations reached
  return false;
}

/*---------------------------------------------------------------*/
/*                      Write functions                          */
/*---------------------------------------------------------------*/
void TIbeeProgrammer::writeBlock(unsigned long addr, byte buf[],
    unsigned short bufLen)
{
  // Make sure the DMA is enabled
  if (!enableDMA) {
    initDMA();
    enableDMA = true;
  }

  // 0. Create DMA descriptors
  // ... Debug Interface -> Buffer
  byte dma_desc_0[8] = {
      highByte(DUP_DBGDATA),    // src[15:8]
      lowByte(DUP_DBGDATA),     // src[7:0]
      highByte(ADDR_BUF0),      // dest[15:8]
      lowByte(ADDR_BUF0),       // dest[7:0]
      highByte(bufLen),         // len[12:8]
      lowByte(bufLen),          // len[7:0]
      31,                       // trigger: DBG_BW
      0x11                      // increment destination
  };

  // ... Buffer -> Flash controller
  byte dma_desc_1[8] = {
      highByte(ADDR_BUF0),      // src[15:8]
      lowByte(ADDR_BUF0),       // src[7:0]
      highByte(DUP_FWDATA),     // dest[15:8]
      lowByte(DUP_FWDATA),      // dest[7:0]
      highByte(bufLen),         // len[12:8]
      lowByte(bufLen),          // len[7:0]
      18,                       // trigger: FLASH
      0x42                      // increment source
  };

  // 1. Write the 2 DMA descriptors to RAM
  writeBlockXDATA(ADDR_DMA_DESC_0, dma_desc_0, 8);
  writeBlockXDATA(ADDR_DMA_DESC_1, dma_desc_1, 8);

  // 2. Set DMA controller pointer to the DAM descriptors
  writeByteXDATA(DUP_DMA0CFGH, highByte(ADDR_DMA_DESC_0));
  writeByteXDATA(DUP_DMA0CFGL, lowByte(ADDR_DMA_DESC_0));
  writeByteXDATA(DUP_DMA1CFGH, highByte(ADDR_DMA_DESC_1));
  writeByteXDATA(DUP_DMA1CFGL, lowByte(ADDR_DMA_DESC_1));

  // 3. Set Flash controller start address (16 MSB of 18 bit address)
  writeByteXDATA(DUP_FADDRH, highByte( (addr >> 2) ));
  writeByteXDATA(DUP_FADDRL, lowByte( (addr >> 2) ));

  // 4. Arm DBG => RAM buffer, DMA channel and start burst write
  writeByteXDATA(DUP_DMAARM, CH_DBG_TO_BUF0);
  burstBlock(buf, bufLen);

  // 5. Start programming: buffer to flash
  writeByteXDATA(DUP_DMAARM, CH_BUF0_TO_FLASH);
  writeByteXDATA(DUP_FCTL, 0x06);

  // 6. Wait until flash controller is done
  while (readByteXDATA(DUP_FCTL) & 0x80);
}

int TIbeeProgrammer::getMaxBufferLength(unsigned long address) {
  return 1024;
}

void TIbeeProgrammer::writeByteXDATA(unsigned short addr, byte value) {
  byte instr[3];

  // MOV DPTR, address
  instr[0] = 0x90;
  instr[1] = highByte(addr);
  instr[2] = lowByte(addr);
  sendInstruction(CMD_DEBUG_INSTR_3, instr, 3);

  // MOV A, value
  instr[0] = 0x74;
  instr[1] = value;
  sendInstruction(CMD_DEBUG_INSTR_2, instr, 2);

  // MOV @DPTR, A
  instr[0] = 0xF0;
  sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);
}

void TIbeeProgrammer::writeBlockXDATA(unsigned short addr, byte buf[],
    unsigned short bufLen) {
  byte instr[3];

  //MOV DPTR, Address
  instr[0] = 0x90;
  instr[1] = highByte(addr);
  instr[2] = lowByte(addr);
  sendInstruction(CMD_DEBUG_INSTR_3, instr, 3);

  for (int i = 0; i < bufLen; i++) {
    //MOV A, values[i]
    instr[0] = 0x74;
    instr[1] = buf[i];
    sendInstruction(CMD_DEBUG_INSTR_2, instr, 2);

    //MOV @DPTR, A
    instr[0] = 0xF0;
    sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);

    // INC DPTR
    instr[0] = 0xA3;
    sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);
  }
}

void TIbeeProgrammer::burstBlock(byte buf[], unsigned short bufLen) {
  // Write command with buffer length
  sendBits(CMD_BURST_WRITE | highByte(bufLen), 8);
  sendBits(lowByte(bufLen), 8);

  // Send the data
  for (int i = 0; i < bufLen; i++)
    sendBits(buf[i], 8);

  receiveBits(8);
}

/*---------------------------------------------------------------*/
/*                       Read functions                          */
/*---------------------------------------------------------------*/
void TIbeeProgrammer::readBlock(unsigned short addr, byte buffer[],
    unsigned short bufLen)
{
  byte instr[3];
  unsigned short xdata_addr = (0x8000 + (addr % 0x8000));
  byte bank = addr / 0x8000;

  // 1. Map flash memory bank to XDATA address 0x8000 - 0xFFFF
  writeByteXDATA(DUP_MEMCTR, bank);

  // 2. Move data pointer to XDATA address (MOV DPTR, xdata_addr)
  instr[0] = 0x90;
  instr[1] = highByte(xdata_addr);
  instr[2] = lowByte(xdata_addr);
  sendInstruction(CMD_DEBUG_INSTR_3, instr, 3);

  for (int i = 0; i < bufLen; i++) {
      // 3. Move value pointed to by DPTR to accumulator (MOVX A, @DPTR)
      instr[0]  = 0xE0;
      buffer[i] = sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);

      // 4. Increment data pointer (INC DPTR)
      instr[0] = 0xA3;
      sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);
  }
}

byte TIbeeProgrammer::readByteXDATA(unsigned short addr) {
  byte instr[3];

  // MOV DPTR, address
  instr[0] = 0x90;
  instr[1] = highByte(addr);
  instr[2] = lowByte(addr);
  sendInstruction(CMD_DEBUG_INSTR_3, instr, 3);

  // MOVX A, @DPTR
  instr[0] = 0xE0;
  return sendInstruction(CMD_DEBUG_INSTR_1, instr, 1);
}
