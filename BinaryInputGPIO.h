#pragma once
#include "BinaryInput.h"

class BinaryInputGPIO : public BinaryInput
{
protected:
  uint8_t mInputPin = 0;
  int8_t mPulsePin = 0;
  bool mInverted = false;

public:
  BinaryInputGPIO(uint8_t iIndex, uint8_t iInputPin, int8_t iPulsePin = -1, bool iInverted = false);
  ~BinaryInputGPIO();

  bool queryHardwareInput();
};
