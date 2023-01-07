#pragma once
#include "knx.h"
#include "Helper.h"
#include "KnxHelper.h"
#include "knxprod.h"
#include "hardware.h"

class BinaryInput
{

protected:
  uint32_t calcParamIndex(uint16_t iParamIndex);
  uint16_t calcKoNumber(uint8_t iKoIndex);
  int8_t calcKoIndex(uint16_t iKoNumber);
  GroupObject *getKo(uint8_t iKoIndex);

  bool debounce();
  void processInput();
  void processPeriodicSend();
  void sendState();

  bool mParamActive;
  uint8_t mParamOpen;
  uint8_t mParamClose;
  uint32_t mParamDebouncing;
  uint32_t mParamPeriodic;
  uint8_t mParamPulsing;

  uint8_t mIndex = 0;

  int8_t mCurrentState = -1;
  int8_t mLastDebounceState = -1;
  int8_t mCurrentHardwareState = -1;

  uint32_t mLastDebounceTime = 0;
  uint32_t mLastPeriodicSend = 0;

public:
  BinaryInput(uint8_t iIndex);
  ~BinaryInput();

  void setup();
  void loop();

  void setHardwareState(bool iState);
  bool isActive();
};
