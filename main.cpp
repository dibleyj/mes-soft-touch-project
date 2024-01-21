/* JD's hacked mbed (bare-metal) blinky
 * Based in large part on chap 3 of A.G. Dean 
 * "Embedded Systems Fundamentals with Arm Cortex-M based Microcontrollers"
 */

#include "mbed.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
InterruptIn sw1(SW1);
InterruptIn sw3(SW3);

volatile bool g_flash_enable{false};
volatile bool g_faster{false};
volatile unsigned g_flash_delay{500};
volatile unsigned g_seque_delay{500};

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
    if (g_faster)
    {
        g_flash_delay = 200;
        g_seque_delay = 200;
    } 
    else 
    {
        g_flash_delay = 500;
        g_seque_delay = 500;
    }
}

void task_flash(void)
{
    if (g_flash_enable)
    {
        bright(led1);
        bright(led2);
        thread_sleep_for(g_flash_delay);
        dark(led2);
        dark(led1);
        thread_sleep_for(g_flash_delay);
    }
}

void task_sequence(void)
{
    static enum {STATE_RED, STATE_GREEN, STATE_OFF} next_state;
    static bool ticktock = false;
    if (!g_flash_enable)
    {
        switch (next_state)
        {
            case STATE_RED:
                bright(led1);
                thread_sleep_for(g_seque_delay);
                next_state = STATE_OFF;
                break;
            case STATE_GREEN:
                bright(led2);
                thread_sleep_for(g_seque_delay);
                next_state = STATE_OFF;
                break;
            case STATE_OFF:
            default:
                dark(led1);
                dark(led2);
                thread_sleep_for(g_seque_delay);
                ticktock = !ticktock;
                if (ticktock) next_state = STATE_GREEN; else next_state = STATE_RED;
                break;
        }        
    }
}

int main()
{
    printf("This is the bare metal blinky example running on Mbed OS %d.%d.%d.\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    sw1.fall(&sw1_handler);
    sw3.fall(&sw3_handler);
    while (true)
    {
        task_flash();
        task_sequence();
    }
}
