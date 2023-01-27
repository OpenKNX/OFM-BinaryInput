#pragma once
#include "OpenKNX.h"

class BinaryInputChannel : public OpenKNX::Channel
{

  protected:
    bool debounce();
    void processInput();
    void processPeriodicSend();
    void sendState();

    bool _paramActive;
    uint8_t _paramOpen;
    uint8_t _paramClose;
    uint32_t _paramDebouncing;
    uint32_t _paramPeriodic;
    uint8_t _paramPulsing;

    int8_t _currentState = -1;
    int8_t _lastDebounceState = -1;
    int8_t _currentHardwareState = -1;

    uint32_t _lastDebounceTime = 0;
    uint32_t _lastPeriodicSend = 0;

  public:
    BinaryInputChannel(uint8_t index);

    void setup();
    void loop();

    void setHardwareState(bool state);
    bool isActive();
    const char* name() override;
};
