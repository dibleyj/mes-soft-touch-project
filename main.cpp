/* JD's hacked mbed (bare-metal) blinky
 * Based in large part on chap 3 of A.G. Dean 
 * "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 */

#include "mbed.h"
#include <chrono>
using namespace std::chrono;

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn sw1(SW1);
InterruptIn sw3(SW3);
Timeout led_timeout;

volatile bool g_flash_enable{false};
volatile bool g_faster{false};

void task_flash(void);
void task_sequence(void);

const std::chrono::microseconds k_led_slow{500000};
const std::chrono::microseconds k_led_fast{200000};

bool pressed(DigitalIn sw)  { return !sw; }
void bright(DigitalOut led) { led = 0; }
void dark(DigitalOut led)   { led = 1; }

void sw1_handler(void)
{
    g_flash_enable = !g_flash_enable;
}

void sw3_handler(void)
{
    g_faster = !g_faster;
}

void reattach_led_timeout(Callback< void() > f)
{
    if (g_faster)
    {
        led_timeout.attach(f, k_led_fast); 
    }
    else  
    {
        led_timeout.attach(f, k_led_slow); 
    }
}

void task_flash(void)
{
    static enum {STATE_ON, STATE_OFF} next_state;
    led_timeout.detach();
    if (g_flash_enable)
    {    
        switch (next_state)
        {
        case STATE_ON:
            bright(led1);
            bright(led2);
            next_state = STATE_OFF;
            break;
        case STATE_OFF:
        default:
            dark(led1);
            dark(led2);
            next_state = STATE_ON;
            break;
        }
        reattach_led_timeout(&task_flash);
    }
    else 
    {
        reattach_led_timeout(&task_sequence);    
    }
}

void task_sequence(void)
{
    static enum {STATE_RED, STATE_GREEN, STATE_OFF} next_state;
    static bool ticktock = false;
    led_timeout.detach();
    if (!g_flash_enable)
    {
        switch (next_state)
        {
        case STATE_RED:
            bright(led1);
            next_state = STATE_OFF;
            break;
        case STATE_GREEN:
            bright(led2);
            next_state = STATE_OFF;
            break;
        case STATE_OFF:
        default:
            dark(led1);
            dark(led2);
            ticktock = !ticktock;
            if (ticktock) next_state = STATE_GREEN; else next_state = STATE_RED;
            break;
        }        
        reattach_led_timeout(&task_sequence);
    }
    else 
    {
        reattach_led_timeout(&task_flash);
    }
}

int main()
{
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    sw1.fall(&sw1_handler);
    sw3.fall(&sw3_handler);
    led_timeout.attach(&task_sequence, k_led_slow);
    while (true)
    {
        thread_sleep_for(200);
        // task_flash();
        // task_sequence();
    }
}
