#include "BinaryInput.h"


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

bool BinaryInput::queryHardwareInput() {
  return false;
};

void BinaryInput::setup()
{
  // Params
  mParamActive = (knx.paramByte(calcParamIndex(BI_InputActive)) & BI_InputActiveMask) >> BI_InputActiveShift;
  mParamOpen = (knx.paramByte(calcParamIndex(BI_InputOpen)) & BI_InputOpenMask) >> BI_InputOpenShift;
  mParamClose = (knx.paramByte(calcParamIndex(BI_InputClose)) & BI_InputCloseMask) >> BI_InputCloseShift;
  mParamDebouncing = (knx.paramByte(calcParamIndex(BI_InputDebouncing)));
  mParamPeriodic = (getDelayPattern(calcParamIndex(BI_InputPeriodicBase)));

  getKo(BI_KoInputOutput)->valueNoSend(false, getDPT(VAL_DPT_1));

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
  if (mParamActive != 1)
    return;

  processInput();
  processPeriodicSend();
}


void BinaryInput::processInput()
{
  // pulsed query
  if (!checkQueryTime())
    return;

  bool lState = queryHardwareInput();

  // Skip till debounced
  if (debounced(lState))
    return;

  if (lState != mCurrentState)
  {
    //SERIAL_DEBUG.printf("BE %i: %i\n\r", mIndex, lState);
    mCurrentState = lState;
    sendState();
  }
}

bool BinaryInput::debounced(bool iCurrentState)
{
  if (iCurrentState != mLastButtonState)
  {
    mLastDebounceTime = millis();
    mLastButtonState = iCurrentState;
  }

  if ((millis() - mLastDebounceTime) > mParamDebouncing)
  {
    mLastButtonState = iCurrentState;
    return false;
  }

  return true;
}

bool BinaryInput::checkQueryTime()
{
  if ((millis() - mLastQueryTime) > BI_QueryDelay)
  {
    mLastQueryTime = millis();
    return true;
  }

  return false;
}

void BinaryInput::processPeriodicSend()
{
  if (mParamPeriodic == 0)
    return;

  if ((millis() - mLastPeriodicSend) > mParamPeriodic)
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

  SERIAL_DEBUG.printf("BE %i: %i\n\r", mIndex, lSendState);
  getKo(BI_KoInputOutput)->value(lSendState, getDPT(VAL_DPT_1));
}