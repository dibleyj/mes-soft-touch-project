#pragma once
#include <cstdint>
#include <cstddef>
#include <chrono>
#include "mbed.h"
#include "mbed_events.h"
#include "USBMIDI.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}

#define ST_VER_MAJOR    0
#define ST_VER_MINOR    0
#define ST_VER_TWEAK    1
#define ST_VER_INFO_STRING ("Soft Touch v0.0.1")

namespace soft_touch 
{

enum STNode : std::uint8_t 
{
    None,
    DevCli,
    Ui,
    UsbMidi,
    SysCtrl,
    numStEventSources
};

enum STEvent : std::uint16_t 
{
    Invalid,
    EventMsgRx,
    EventMsgRxFail,
    DevCliDisplayTest,
    UiEncoderTurn,
    numSTEvents
};

struct STEventMessage
{
    STNode src;
    STNode dst;
    STEvent e;
    int32_t v;
};

}