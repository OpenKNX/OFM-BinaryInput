#include "BinaryInputChannel.h"

BinaryInputChannel::BinaryInputChannel(uint8_t index)
{
    _channelIndex = index;
    _channelParamBlockSize = BI_ParamBlockSize;
    _channelParamOffset = BI_ParamBlockOffset;
    _channelParamKoBlockSize = BI_KoBlockSize;
    _channelParamKoOffset = BI_KoOffset;
}

const char* BinaryInputChannel::name()
{
    char* buffer = new char[22];
    sprintf(buffer, "BinaryInputChannel<%i>", _channelIndex + 1);
    return buffer;
}

void BinaryInputChannel::setup()
{
    // Params
    _paramActive = (knx.paramByte(calcParamIndex(BI_ChannelActive)) & BI_ChannelActiveMask) >> BI_ChannelActiveShift;
    _paramOpen = (knx.paramByte(calcParamIndex(BI_ChannelOpen)) & BI_ChannelOpenMask) >> BI_ChannelOpenShift;
    _paramClose = (knx.paramByte(calcParamIndex(BI_ChannelClose)) & BI_ChannelCloseMask) >> BI_ChannelCloseShift;
    _paramDebouncing = (knx.paramByte(calcParamIndex(BI_ChannelDebouncing)));
    _paramPeriodic = (getDelayPattern(calcParamIndex(BI_ChannelPeriodicBase)));
    _paramPeriodic = 0;

    getKo(BI_KoChannelOutput)->valueNoSend(false, getDPT(VAL_DPT_1));

// Debug
#ifdef BI_DEBUG
    debug("paramActive: %i", _paramActive);
    debug("paramOpen: %i", _paramOpen);
    debug("paramClose: %i", _paramClose);
    debug("paramDebouncing: %i", _paramDebouncing);
    debug("paramPeriodic: %i", _paramPeriodic);
#endif
}
void BinaryInputChannel::loop()
{
    if (!_paramActive)
        return;

    processInput();
    processPeriodicSend();
}

void BinaryInputChannel::setHardwareState(bool state)
{
    if (state == _currentHardwareState)
        return;

#ifdef BI_DEBUG
    debug("setHardwareState %i", state);
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
    if (_paramDebouncing == 0)
        return false;

    if (_currentHardwareState != _lastDebounceState)
    {
        _lastDebounceTime = millis();
        _lastDebounceState = _currentHardwareState;
    }

    if (delayCheck(_lastDebounceTime, _paramDebouncing))
    {
        _lastDebounceState = _currentHardwareState;
        return false;
    }

    return true;
}

void BinaryInputChannel::processPeriodicSend()
{
    // no hw state available
    if (_currentState == -1)
        return;

    // periodic send is disabled
    if (_paramPeriodic == 0)
        return;

    // first run - skip
    if (_lastPeriodicSend == 0)
    {
        _lastPeriodicSend = millis();
        return;
    }

    if (delayCheck(_lastPeriodicSend, _paramPeriodic))
    {
        _lastPeriodicSend = millis();
        sendState();
    }
}

void BinaryInputChannel::sendState()
{
    int8_t state = -1;

    if (_currentState && _paramClose == 1) // Open - send 0
        state = false;

    if (_currentState && _paramClose == 2) // Closed - send 1
        state = true;

    if (!_currentState && _paramOpen == 1) // OPen - send 0
        state = false;

    if (!_currentState && _paramOpen == 2) // Closed - send 1
        state = true;

    if (state == -1)
        return;

#ifdef BI_DEBUG
    debug("sendState: %i", state);
#endif
    getKo(BI_KoChannelOutput)->value(state, getDPT(VAL_DPT_1));
}

bool BinaryInputChannel::isActive()
{
    return _paramActive;
}