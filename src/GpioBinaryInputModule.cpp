#include "GpioBinaryInputModule.h"

#if defined(OPENKNX_BI_GPIO_PINS) && OPENKNX_BI_GPIO_COUNT > 0 && BI_ChannelCount > 0

const std::string GpioBinaryInputModule::name()
{
    return "BasicBinaryInput";
}

const std::string GpioBinaryInputModule::version()
{
    return MODULE_BinaryInput_Version;
}

void GpioBinaryInputModule::setup()
{
    if (_gpioPins == nullptr)
    {
        logErrorP("No GPIO pins defined, module inactive");
        return;
    }

    for (uint8_t i = 0; i < MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT); i++)
    {
#if OPENKNX_BI_ONLEVEL == LOW
        openknx.gpio.pinMode(_gpioPins[i], INPUT_PULLUP);
#else // OPENKNX_BI_ONLEVEL == HIGH
        openknx.gpio.pinMode(_gpioPins[i], INPUT_PULLDOWN);
#endif

#if defined(OPENKNX_BI_PULSE) && OPENKNX_BI_PULSE != -1
        openknx.gpio.pinMode(OPENKNX_BI_PULSE, OUTPUT);
#endif

        _channels[i] = new BinaryInputChannel(i);
        _channels[i]->setup();
    }
}

void GpioBinaryInputModule::loop()
{
    if (_gpioPins == nullptr)
        return;

    processHardwareInputs();

    for (uint8_t i = 0; i < MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT); i++)
        _channels[i]->loop();
}

void GpioBinaryInputModule::processHardwareInputs()
{
    if (_gpioPins == nullptr)
        return;

#if defined(OPENKNX_BI_PULSE) && OPENKNX_BI_PULSE != -1
    if (!delayCheck(_lastHardwareQuery, OPENKNX_BI_PULSE_PAUSE_TIME))
        return;

    openknx.gpio.digitalWrite(OPENKNX_BI_PULSE, true);
    delayMicroseconds(OPENKNX_BI_PULSE_WAIT_TIME);
#endif

    for (uint8_t i = 0; i < MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT); i++)
    {
        if (_channels[i]->isActive())
            _channels[i]->setHardwareState(openknx.gpio.digitalRead(_gpioPins[i]) == OPENKNX_BI_ONLEVEL);
    }

#if defined(OPENKNX_BI_PULSE) && OPENKNX_BI_PULSE != -1
    openknx.gpio.digitalWrite(OPENKNX_BI_PULSE, false);
    _lastHardwareQuery = millis();
#endif
}

GpioBinaryInputModule openknxGpioBinaryInputModule;

#endif
