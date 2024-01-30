#pragma once
#include "soft_touch.h"
#include "uistateinterface.h"

namespace soft_touch {

    enum ButtonState  
    {
        Unpressed,
        Pressed,
        PressedAndHeld,
        Released,
        ReleasedFromHold,
        numButtonStates  
    };

    class Button {
    public:
        Button(PinName pn, ButtonId button)
            : d_i(pn)
            , id(button)
            , button_state(Unpressed)
        {

        }
        ~Button(void);


        void Debounce()
        {
            static uint8_t hold_counter{0};
            switch_state = (switch_state << 1) | d_i;
            switch (button_state)
            {
            case Unpressed:
                if (pressed()) UpdateButtonState(Pressed);
                break;
            case Pressed:
                if (pressed()) 
                {
                    ++hold_counter;
                    if (hold_counter >= k_hold_timeout)
                    {
                        hold_counter = 0;
                        UpdateButtonState(PressedAndHeld);
                    }
                }
                else 
                {
                    hold_counter = 0;
                    UpdateButtonState(Released);
                    UpdateButtonState(Unpressed);
                }
                break;
            case PressedAndHeld:
                if (!pressed())
                {
                    UpdateButtonState(ReleasedFromHold);
                    UpdateButtonState(Unpressed);
                }
                break; 
            default:
                break;
            }
        }

        void UpdateButtonState(ButtonState new_state);

        inline bool pressed() const { return switch_state == 0x00; }

        inline bool released() const { return switch_state == 0x7f; }

        void operator=(const Button&) = delete;

    private:
        ButtonId id;
        DigitalIn d_i;
        uint8_t switch_state;
        ButtonState button_state; 
        uint8_t k_hold_timeout{0xC0}; // k_hold_timeout * 125us = 24ms
    };
}