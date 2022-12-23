#include "BinaryInputGPIO.h"

BinaryInputGPIO::BinaryInputGPIO(uint8_t iIndex, uint8_t iInputPin, int8_t iPulsePin, bool iInverted)
{
  mIndex = iIndex;
  mInputPin = iInputPin;
  mPulsePin = iPulsePin;
  mInverted = iInverted;
}

bool BinaryInputGPIO::queryHardwareInput()
{
  if (mPulsePin >= 0)
    digitalWrite(mPulsePin, true);

  bool lState = digitalRead(mInputPin);

  if (mPulsePin >= 0)
    digitalWrite(mPulsePin, false);

  if (
    (!mInverted && lState == LOW) ||
    (mInverted && lState == HIGH))
    return true;

  return false;
}
