#include "ui.h"

namespace soft_touch
{
    Led Ui::leds[Ui::numUiLights] = 
    {
        Led(LED1),
        Led(LED2)
    };

    Button Ui::buttons[Ui::numUiButtons] = 
    {
        Button(SW3), 
        Button(SW1),
        Button(D4)
    };


}