#include "BinaryInputChannel.h"

BinaryInputChannel::BinaryInputChannel(uint8_t index)
{
    _channelIndex = index;
}

const std::string BinaryInputChannel::name()
{
    return "BinaryInput";
}

void BinaryInputChannel::setup()
{
    if (!ParamBI_ChannelActive)
        return;

    KoBI_ChannelOutput.valueNoSend(false, DPT_Switch);

// Debug
#ifdef TRACE_BINARY_INPUT
    log("paramActive: %i", ParamBI_ChannelActive);
    log("paramOpen: %i", ParamBI_ChannelOpen);
    log("paramClose: %i", ParamBI_ChannelClose);
    log("paramDebouncing: %i", ParamBI_ChannelDebouncing);
    log("paramPeriodic: %i", ParamBI_ChannelPeriodicTimeMS);
#endif
}
void BinaryInputChannel::loop()
{
    if (!ParamBI_ChannelActive)
        return;

    processInput();
    processPeriodicSend();
}

void BinaryInputChannel::setHardwareState(bool state)
{
    if (!ParamBI_ChannelActive)
        return;

    if (state == _currentHardwareState)
        return;

#ifdef TRACE_BINARY_INPUT
    log("setHardwareState %i", state);
#endif
    _currentHardwareState = state;
}

void BinaryInputChannel::processInput()
{
    // no hw state available
    if (_currentHardwareState == -1)
        return;

    if (debounce())
        return;

    if (_currentHardwareState != _currentState)
    {

        _currentState = _currentHardwareState;
        sendState();
    }
}

bool BinaryInputChannel::debounce()
{
    // Skip is debouncing disabled
    if (ParamBI_ChannelDebouncing == 0)
        return false;

    if (_currentHardwareState != _lastDebounceState)
    {
        _lastDebounceTime = millis();
        _lastDebounceState = _currentHardwareState;
    }

    if (delayCheck(_lastDebounceTime, ParamBI_ChannelDebouncing))
    {
        _lastDebounceState = _currentHardwareState;
        return false;
    }

    return true;
}

void BinaryInputChannel::processPeriodicSend()
{
    uint32_t paramValue = ParamBI_ChannelPeriodicTimeMS;

    // no hw state available
    if (_currentState == -1)
        return;

    // periodic send is disabled
    if (paramValue == 0)
        return;

    // first run - skip
    if (_lastPeriodicSend == 0)
    {
        _lastPeriodicSend = millis();
        return;
    }

    if (delayCheck(_lastPeriodicSend, paramValue))
    {
        _lastPeriodicSend = millis();
        sendState();
    }
}

void BinaryInputChannel::sendState()
{
    int8_t state = -1;

    if (_currentState && ParamBI_ChannelClose == 1) // Open - send 0
        state = false;

    if (_currentState && ParamBI_ChannelClose == 2) // Closed - send 1
        state = true;

    if (!_currentState && ParamBI_ChannelOpen == 1) // OPen - send 0
        state = false;

    if (!_currentState && ParamBI_ChannelOpen == 2) // Closed - send 1
        state = true;

    if (state == -1)
        return;

#ifdef TRACE_BINARY_INPUT
    log("sendState: %i", state);
#endif
    KoBI_ChannelOutput.value(state, DPT_Switch);
}

bool BinaryInputChannel::isActive()
{
    return ParamBI_ChannelActive;
}