/* JD's hacked mbed (bare-metal) blinky
 * Based in part on chap 3 of A.G. Dean's
 * "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 */

#include "soft_touch.h"
#include "STLCD.h"
#include "console.h"
using namespace std::chrono;
using namespace soft_touch;

DigitalOut led1(LED1);
Ticker sys_tick; 
USBMIDI midi;

void heartbeat()
{
    static uint16_t counter = 0;
    ++counter;
    if (counter > 0 && counter <= 0x100)
    {
        led1 = 0;
    }
    else if (counter < 0x2710)
    {
        led1 = 1;
        // STLCD::instance().queue();
    } 
    else 
    {
        counter = 0;
    }
}


int main()
{
    printf("JD's Soft Touch running on bare-metal Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    Console dev_console;
    dev_console.Init();
    sys_tick.attach(&heartbeat, 100us);

    static uint8_t whatever{0};
    while(1) 
    {
        ThisThread::sleep_for(250ms);
        dev_console.Process();
        STLCD::instance().WriteUint8H(STLCD::LH, whatever);
        STLCD::instance().WriteUint8H(STLCD::RH, UINT8_MAX - whatever);
        // STLCD::instance().lcd_queue.call()
        midi.write(MIDIMessage::NoteOn(64));
        ThisThread::sleep_for(150ms);
        midi.write(MIDIMessage::NoteOff(64));
        whatever++;
    }
}

