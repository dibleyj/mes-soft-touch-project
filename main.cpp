/* JD's Soft Touch
 */

#include "soft_touch.h"
#include "ui.h"
#include "console.h"
#include "system.h"

using namespace std::chrono;
using namespace soft_touch;

Ticker sys_tick; 
// USBMIDI midi;

void SystemTick()
{
    // 1kHz polling
    static uint8_t counter = 0;
    if ((++counter & 7) == 0) 
    {
        Ui::instance().Poll(); 
    }
    // 8kHz polling

}

int main()
{
    Console::instance().Init();
    SystemController::instance().Init();
    Ui::instance().Init();
    sys_tick.attach(&SystemTick, 125us);
    while(1) 
    {
        ThisThread::sleep_for(1ms);
        Console::instance().Process();
        SystemController::instance().Process();
        Ui::instance().Process();
    }
}

