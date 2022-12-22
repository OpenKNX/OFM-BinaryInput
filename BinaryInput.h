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

  bool debounced(bool iCurrentState);
  void processInput();
  void processPeriodicSend();
  bool checkQueryTime();
  void sendState();

  uint8_t mParamActive;
  uint8_t mParamOpen;
  uint8_t mParamClose;
  uint32_t mParamDebouncing;
  uint32_t mParamPeriodic;
  uint8_t mParamPulsing;

  uint8_t mIndex = 0;

  int8_t mCurrentState = -1;
  int8_t mLastButtonState = -1;
  uint32_t mLastDebounceTime = 0;
  uint32_t mLastQueryTime = 0;
  uint32_t mLastPeriodicSend = 0;

public:
  virtual void setup();
  virtual void loop();

  virtual bool queryHardwareInput();
};
