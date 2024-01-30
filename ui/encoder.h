#pragma once
#include "mbed.h"
#include "soft_touch.h"
#include "QEI.h"
#include "uistateinterface.h"

namespace soft_touch
{
 

    class Encoder 
    {
    public:
        Encoder(EncoderId id, PinName chanA, PinName chanB) 
            : id(id), 
              enc(chanA, chanB, NC, 24, QEI::X2_ENCODING)
        {

        }

        ~Encoder(){}

        void 
        GetDelta();

    private:
        EncoderId id; 
        QEI enc;
        int32_t pulses{0};
        int32_t last_pulses{0};
        

    };
}