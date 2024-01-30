#pragma once
#include "soft_touch.h"
#include "uistateinterface.h"

namespace soft_touch {

    class Button {
    public:
        Button(PinName pn)
            : d_i(pn)
            , button_state(Unpressed)
        {

        }
        ~Button(void);

        enum Status  
        {
            Unpressed,
            Pressed,
            Held,
            Released,
            numButtonStates  
        };

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
                        UpdateButtonState(Held);
                    }
                }
                else 
                {
                    hold_counter = 0;
                    UpdateButtonState(Released);
                }
                break;
            case Held:
                if (!pressed())
                {
                    UpdateButtonState(Released);
                }
                break; 
            case Released: 
                if (pressed())
                {
                    UpdateButtonState(Pressed);
                } 
                else 
                {
                    UpdateButtonState(Unpressed);
                }
            default:
                break;
            }
        }

        void UpdateButtonState(Status new_state)
        {
            // printf("UpdateButtonState, %i -> %i\r\n", button_state, new_state);
            button_state = new_state;
            // TODO: call method on Ui->state, which will then post a message to SysCtrl
        }

        inline bool pressed() const { return switch_state == 0x00; }

        inline bool just_pressed() const { return switch_state == 0x80; }

        inline bool released() const { return switch_state == 0x7f; }

        void operator=(const Button&) = delete;

    private:
        DigitalIn d_i;
        uint8_t switch_state;
        Status button_state; 
        uint8_t k_hold_timeout{0xC0}; // k_hold_timeout * 125us = 24ms
    };
}