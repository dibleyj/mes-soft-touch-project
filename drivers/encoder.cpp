#include "encoder.h"
#include "ui.h"

namespace soft_touch
{
    
void Encoder::GetDelta()
{
    int8_t delta{0};
    pulses = enc.getPulses();
    if (pulses != last_pulses) 
    {
        (pulses > last_pulses) ? (delta = 1) : (delta = -1);
        last_pulses = pulses;         
        Ui::instance().current_mode->Turn(id, delta);
    }        
}


};