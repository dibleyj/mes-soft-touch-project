#include "ui.h"
#include "button.h"

namespace soft_touch 
{

void Button::UpdateButtonState(ButtonState new_state)
{
    // printf("UpdateButtonState, %i -> %i\r\n", button_state, new_state);
    button_state = new_state;
    switch (button_state)
    {
    case PressedAndHeld:
        Ui::instance().current_mode->PressAndHold(id);
        break;
    case Released:
        Ui::instance().current_mode->Release(id);
        break;        
    case ReleasedFromHold:
        Ui::instance().current_mode->ReleaseFromHold(id);
        break;
    default:
        break;
    }

}

};