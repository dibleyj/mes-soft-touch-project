#pragma once
#include <cstdint>
#include <cstddef>
#include <chrono>
#include "mbed.h"
#include "mbed_events.h"
#include "USBMIDI.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}

enum STEventSource : std::uint16_t 
{
    None,
    DevCli,
    Ui,
    UsbMidi,
    numStEventSources
};

enum STEvent : std::uint16_t 
{
    DevCliDisplayTest,
    numSTEvents
};