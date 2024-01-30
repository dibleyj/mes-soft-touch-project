#pragma once
#include "soft_touch.h"

namespace soft_touch {
    class Led {
 
    public:
        Led(PinName pn)
            : d_o(pn)
        {
            d_o = 1;
        }
        ~Led(void);

        void Light(bool on) { on ? (d_o = 0) : (d_o = 1); }


        void operator=(const Led&) = delete;

    private:
        DigitalOut d_o;

    };
}