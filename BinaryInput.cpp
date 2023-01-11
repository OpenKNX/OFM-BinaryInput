#include "BinaryInput.h"

BinaryInput::BinaryInput(uint8_t iIndex)
{
  mIndex = iIndex;
}

uint32_t BinaryInput::calcParamIndex(uint16_t iParamIndex)
{
  return iParamIndex + mIndex * BI_ParamBlockSize + BI_ParamBlockOffset;
}

uint16_t BinaryInput::calcKoNumber(uint8_t iKoIndex)
{
  return iKoIndex + mIndex * BI_KoBlockSize + BI_KoOffset;
}

int8_t BinaryInput::calcKoIndex(uint16_t iKoNumber)
{
  int16_t result = (iKoNumber - BI_KoOffset);
  // check if channel is valid
  if ((int8_t)(result / BI_KoBlockSize) == mIndex)
    result = result % BI_KoBlockSize;
  else
    result = -1;
  return (int8_t)result;
}

GroupObject *BinaryInput::getKo(uint8_t iKoIndex)
{
  return &knx.getGroupObject(calcKoNumber(iKoIndex));
}

void BinaryInput::setup()
{
  // Params
  mParamActive = (knx.paramByte(calcParamIndex(BI_ChannelActive)) & BI_ChannelActiveMask) >> BI_ChannelActiveShift;
  mParamOpen = (knx.paramByte(calcParamIndex(BI_ChannelOpen)) & BI_ChannelOpenMask) >> BI_ChannelOpenShift;
  mParamClose = (knx.paramByte(calcParamIndex(BI_ChannelClose)) & BI_ChannelCloseMask) >> BI_ChannelCloseShift;
  mParamDebouncing = (knx.paramByte(calcParamIndex(BI_ChannelDebouncing)));
  mParamPeriodic = (getDelayPattern(calcParamIndex(BI_ChannelPeriodicBase)));

  getKo(BI_KoChannelOutput)->valueNoSend(false, getDPT(VAL_DPT_1));

// Debug
#ifdef BI_DEBUG
  SERIAL_DEBUG.printf("BE %i mParamActive: %i\n\r", mIndex, mParamActive);
  SERIAL_DEBUG.printf("BE %i mParamOpen: %i\n\r", mIndex, mParamOpen);
  SERIAL_DEBUG.printf("BE %i mParamClose: %i\n\r", mIndex, mParamClose);
  SERIAL_DEBUG.printf("BE %i mParamDebouncing: %i\n\r", mIndex, mParamDebouncing);
  SERIAL_DEBUG.printf("BE %i mParamPeriodic: %i\n\r", mIndex, mParamPeriodic);
#endif
}
void BinaryInput::loop()
{
  if (!mParamActive)
    return;

  processInput();
  processPeriodicSend();
}

void BinaryInput::setHardwareState(bool iState)
{
  if (iState == mCurrentHardwareState)
    return;

#ifdef BI_DEBUG
  SERIAL_DEBUG.printf("BE %i: HardwareState %i\n\r", mIndex, iState);
#endif
  mCurrentHardwareState = iState;
}

void BinaryInput::processInput()
{
  // no hw state available
  if (mCurrentHardwareState == -1)
    return;

  if (debounce())
    return;

  if (mCurrentHardwareState != mCurrentState)
  {

    mCurrentState = mCurrentHardwareState;
    sendState();
  }
}

bool BinaryInput::debounce()
{
  // Skip is debouncing disabled
  if (mParamDebouncing == 0)
    return false;

  if (mCurrentHardwareState != mLastDebounceState)
  {
    mLastDebounceTime = millis();
    mLastDebounceState = mCurrentHardwareState;
  }

  if (delayCheck(mLastDebounceTime, mParamDebouncing))
  {
    mLastDebounceState = mCurrentHardwareState;
    return false;
  }

  return true;
}

void BinaryInput::processPeriodicSend()
{
  // no hw state available
  if (mCurrentState == -1)
    return;

  // periodic send is disabled
  if (mParamPeriodic == 0)
    return;

  // first run - skip
  if (mLastPeriodicSend == 0)
  {
    mLastPeriodicSend = millis();
    return;
  }

  if (delayCheck(mLastPeriodicSend, mParamPeriodic))
  {
    mLastPeriodicSend = millis();
    sendState();
  }
}

void BinaryInput::sendState()
{
  int8_t lSendState = -1;

  if (mCurrentState && mParamClose == 1) // Open - send 0
    lSendState = false;

  if (mCurrentState && mParamClose == 2) // Closed - send 1
    lSendState = true;

  if (!mCurrentState && mParamOpen == 1) // OPen - send 0
    lSendState = false;

  if (!mCurrentState && mParamOpen == 2) // Closed - send 1
    lSendState = true;

  if (lSendState == -1)
    return;

#ifdef BI_DEBUG
  SERIAL_DEBUG.printf("BE %i: %i\n\r", mIndex, lSendState);
#endif
  getKo(BI_KoChannelOutput)->value(lSendState, getDPT(VAL_DPT_1));
}

bool BinaryInput::isActive()
{
  return mParamActive;
}