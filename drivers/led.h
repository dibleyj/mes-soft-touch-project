#pragma once
#include "soft_touch.h"

namespace soft_touch {




    class Led {
 
    public:
        Led(PinName pn, LightId id)
            :   d_o(pn),
                id(id)
        {
            d_o = 1;
        }

        ~Led(void);

        void Light(bool on) { on ? (d_o = 0) : (d_o = 1); }

        void operator=(const Led&) = delete;

    private:
        DigitalOut d_o;
        LightId id;

    };
}