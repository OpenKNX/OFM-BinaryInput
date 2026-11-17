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

#ifdef OPENKNX_BI_DYNAMIC_PINS
bool GpioBinaryInputModule::configureChannel(uint8_t channel, uint16_t gpio, uint8_t mode, bool activeHigh)
{
    if (_started || channel >= OPENKNX_BI_GPIO_COUNT || gpio > 255 ||
        (mode != INPUT && mode != INPUT_PULLUP && mode != INPUT_PULLDOWN))
        return false;
    _gpioPins[channel] = gpio;
    _inputModes[channel] = mode;
    _activeHigh[channel] = activeHigh;
    return true;
}
#endif

void GpioBinaryInputModule::setup()
{
    if (_gpioPins == nullptr)
    {
        logErrorP("No GPIO pins defined, module inactive");
        return;
    }

#ifdef OPENKNX_BI_DYNAMIC_PINS
    _started = true;
#endif
    uint8_t active = 0;
    for (uint8_t i = 0; i < MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT); i++)
    {

        if (_gpioPins[i] == 255)
            continue;
        _channels[i] = new BinaryInputChannel(i);
        _channels[i]->setup();
        if (_channels[i]->isActive())
        {
            active++;
        #ifdef OPENKNX_BI_DYNAMIC_PINS
            openknx.gpio.pinMode(_gpioPins[i], _inputModes[i]);
        #elif OPENKNX_BI_ONLEVEL == LOW
            openknx.gpio.pinMode(_gpioPins[i], INPUT_PULLUP);
        #else // OPENKNX_BI_ONLEVEL == HIGH
            openknx.gpio.pinMode(_gpioPins[i], INPUT_PULLDOWN);
        #endif
        }
    }
    #if defined(OPENKNX_BI_PULSE) && OPENKNX_BI_PULSE != -1
        openknx.gpio.pinMode(OPENKNX_BI_PULSE, OUTPUT);
    #endif
    logInfoP("Setup completed with %u/%u channels active", active, MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT));
}

void GpioBinaryInputModule::loop()
{
    if (_gpioPins == nullptr)
        return;

    processHardwareInputs();

    for (uint8_t i = 0; i < MIN(BI_ChannelCount, OPENKNX_BI_GPIO_COUNT); i++)
        if (_channels[i] != nullptr)
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
        if (_channels[i] != nullptr && _channels[i]->isActive())
        {
#ifdef OPENKNX_BI_DYNAMIC_PINS
            _channels[i]->setHardwareState(openknx.gpio.digitalRead(_gpioPins[i]) == _activeHigh[i]);
#else
            _channels[i]->setHardwareState(openknx.gpio.digitalRead(_gpioPins[i]) == OPENKNX_BI_ONLEVEL);
#endif
        }
    }

#if defined(OPENKNX_BI_PULSE) && OPENKNX_BI_PULSE != -1
    openknx.gpio.digitalWrite(OPENKNX_BI_PULSE, false);
    _lastHardwareQuery = millis();
#endif
}

GpioBinaryInputModule openknxGpioBinaryInputModule;

#endif
