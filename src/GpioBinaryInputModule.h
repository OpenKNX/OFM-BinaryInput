#pragma once

#include "BinaryInputChannel.h"
#include "OpenKNX/Module.h"
#include "hardware.h"

#define OPENKNX_LEDFUNC_BI_ACT 600  // Binary Input Central Activity
#define OPENKNX_LEDFUNC_BI_STATUS 700
#define OPENKNX_LEDFUNC_BI_CHANNEL_ACT (OPENKNX_LEDFUNC_BI_ACT + _channelIndex + 1)
#define OPENKNX_LEDFUNC_BI_CHANNEL_STATUS (OPENKNX_LEDFUNC_BI_STATUS + _channelIndex + 1)

#if defined(OPENKNX_BI_GPIO_PINS) && OPENKNX_BI_GPIO_COUNT > 0 && BI_ChannelCount > 0

#ifndef OPENKNX_BI_ONLEVEL
#define OPENKNX_BI_ONLEVEL LOW
#endif

class GpioBinaryInputModule : public OpenKNX::Module
{

  public:
    const std::string name() override;
    const std::string version() override;
    void loop() override;
    void setup() override;
#ifdef OPENKNX_BI_DYNAMIC_PINS
    // Call before setup. GPIO 255 disables the channel; settings are supplied by the OAM.
    bool configureChannel(uint8_t channel, uint16_t gpio, uint8_t mode, bool activeHigh);
#endif

  private:
    void processHardwareInputs();

    uint32_t _lastHardwareQuery = 0;

    uint16_t _gpioPins[OPENKNX_BI_GPIO_COUNT] = {OPENKNX_BI_GPIO_PINS};

    BinaryInputChannel* _channels[OPENKNX_BI_GPIO_COUNT] = {};
#ifdef OPENKNX_BI_DYNAMIC_PINS
    uint8_t _inputModes[OPENKNX_BI_GPIO_COUNT] = {};
    bool _activeHigh[OPENKNX_BI_GPIO_COUNT] = {};
    bool _started = false;
#endif
};

extern GpioBinaryInputModule openknxGpioBinaryInputModule;

#endif
