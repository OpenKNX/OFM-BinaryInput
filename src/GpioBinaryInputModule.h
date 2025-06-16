#include "BinaryInputChannel.h"
#include "OpenKNX/Module.h"
#include "hardware.h"

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

  private:
    void processHardwareInputs();

    uint32_t _lastHardwareQuery = 0;

    const uint16_t _gpioPins[OPENKNX_BI_GPIO_COUNT] = {OPENKNX_BI_GPIO_PINS};

    BinaryInputChannel* _channels[OPENKNX_BI_GPIO_COUNT];
};

extern GpioBinaryInputModule openknxGpioBinaryInputModule;

#endif
