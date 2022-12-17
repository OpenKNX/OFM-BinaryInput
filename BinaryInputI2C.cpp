#include "BinaryInputI2C.h"

BinaryInputI2C::BinaryInputI2C(uint8_t iIndex)
{
  mIndex = iIndex;
}

bool BinaryInputI2C::queryHardwareInput()
{
  return false;
}
