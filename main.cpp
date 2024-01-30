/* JD's Soft Touch
 */

#include "soft_touch.h"
#include "ui.h"
#include "console.h"
#include "system.h"

using namespace std::chrono;
using namespace soft_touch;


Ticker sys_tick; 
USBMIDI midi;
// int32_t pulses; 

void SystemTick()
{
    static uint8_t counter = 0;
    if ((++counter & 7) == 0) 
    {
        Ui::instance().Poll();
    }
    // if (counter > 0 && counter <= 0x100)
    // {
    //     Ui::instance().Light(Ui::UiLight01, true);
    // }
    // else if (counter < 0x2710)
    // {
    //     Ui::instance().Light(Ui::UiLight01, false);
    // } 
    // else 
    // {
    //     counter = 0;
    // }
}


int main()
{
    Console::instance().Init();
    SystemController::instance().Init();
    Ui::instance().Init();
    sys_tick.attach(&SystemTick, 100us);
    while(1) 
    {
        ThisThread::sleep_for(1ms);
        Console::instance().Process();
        SystemController::instance().Process();
        Ui::instance().Process();
        // STLCD::instance().WriteUint8H(STLCD::LH, whatever);
        // STLCD::instance().WriteUint8H(STLCD::RH, UINT8_MAX - whatever);
        
        // midi.write(MIDIMessage::NoteOn(64));
        // ThisThread::sleep_for(150ms);
        // midi.write(MIDIMessage::NoteOff(64));
        // whatever++;
        // if (pulses != pulses_memory) printf("Pulses on ST encoder are: %i\n", pulses);
        // pulses_memory = pulses;
    }
}

