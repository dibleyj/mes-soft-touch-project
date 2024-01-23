#pragma once
#include "soft_touch.h"

namespace soft_touch {
    class Led {
    private:
        DigitalOut d_o;

    public:
        Led(PinName pn)
            : d_o(pn)
        {

        }
        ~Led(void);
        Led(const Led&) = delete;
        void operator=(const Led&) = delete;
    };
}