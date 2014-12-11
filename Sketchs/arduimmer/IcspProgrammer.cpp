#include "Arduino.h"
#include "IcspProgrammer.h"

IcspProgrammer::IcspProgrammer(int dataPin, int clockPin)
{
  _dataPin = dataPin;
  _clockPin = clockPin;
  
  init();
}

void IcspProgrammer::init()
{
  pinMode(_dataPin, OUTPUT);
  pinMode(_clockPin, OUTPUT);
}
