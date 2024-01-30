#include "ui.h"

namespace soft_touch
{
    Led Ui::leds[numUiLights] = 
    {
        Led(LED1, BoardGreenLed),
        Led(LED2, BoardRedLed)
    };

    Button Ui::buttons[numUiButtons] = 
    {
        Button(SW3, BoardSwLH), 
        Button(SW1, BoardSwRH),
        Button(D4, EncoderSw)
    };

    Encoder main_enc(MainEncoder, D2, D3);

    Encoder* Ui::encoders[numUiEncoders] =
    {
        &main_enc
    };

}