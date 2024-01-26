/* JD's hacked mbed (bare-metal) blinky
 * Based in part on chap 3 of A.G. Dean's
 * "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 */

#include "soft_touch.h"
#include "console.h"
#include <cstdint>
using namespace std::chrono;
using namespace soft_touch;

// namespace soft_touch {

// #define BUFF_LENGTH 128
DigitalOut led1(LED1);
DigitalOut led2(LED2);
Ticker sys_tick; 

// uint8_t rx_buf[BUFF_LENGTH] = {0};


void heartbeat()
{
    static uint16_t counter = 0;
    if ((++counter & 0x0FFF) == 0) {
        led2 = !led2;
    }
}

// }

int main()
{

    printf("JD's Soft Touch running on bare-metal Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    Console dev_console;
    dev_console.Init();
    sys_tick.attach(&heartbeat, 100us);
    while(1) 
    {
        ThisThread::sleep_for(50ms);
        // led1 =  !led1;
        dev_console.Process();
    }
}

