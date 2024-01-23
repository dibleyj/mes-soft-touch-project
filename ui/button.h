#pragma once
#include "soft_touch.h"

namespace soft_touch {

    class Button {
    private:
        DigitalIn d_i;

    public:
        Button(PinName pn)
            : d_i(pn)
        {

        }
        ~Button(void);
        Button(const Button&) = delete;
        void operator=(const Button&) = delete;
    };
}