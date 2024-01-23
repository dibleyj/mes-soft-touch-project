#pragma once
#include "soft_touch.h"
#include "button.h"
#include "led.h"

namespace soft_touch {

class Ui {
    private: 
        Button kl_button_1;
        Button kl_button_2;
        Led kl_led_g;
        Led kl_led_r;

    public: 
    Ui() :  kl_button_1(SW1),
            kl_button_2(SW3),
            kl_led_g(LED1),
            kl_led_r(LED2)
            {}    

};

}