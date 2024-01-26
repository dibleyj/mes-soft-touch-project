#pragma once
#include <cstdint>
#include <chrono>
#include "mbed.h"
#include "mbed_events.h"

#define IGNORE_UNUSED_VARIABLE(x)     if ( &x == &x ) {}

enum STEventSource 
{
    None,
    DevCli,
    Ui,
    UsbMidi,
    numStEventSources
};

enum STEvent
{
    DevCliDisplayTest,
    numSTEvents
};