#pragma once
#include "BinaryInput.h"

class BinaryInputI2C : public BinaryInput
{
public:
  BinaryInputI2C(uint8_t iIndex);
  ~BinaryInputI2C();

  bool queryHardwareInput();
};
