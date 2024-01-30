#pragma once
#include "soft_touch.h"
#include "QEI.h"
#include "STLCD.h"
#include "led.h"
#include "button.h"
#include "system.h"
#include "encoder.h"
#include "uistateinterface.h"
#include "uicfgstate.h"
#include "uictrlstate.h"
#include <cstdio>

using namespace std;

namespace soft_touch {

    class Ui {
    public:
        static Ui & 
        instance()
        {
            static Ui m_instance;
            return m_instance;
        }

        void Init()
        {
            Light(BoardGreenLed, false);
            Light(BoardRedLed, false);
        }

        void Poll()
        {
            for (uint8_t i = 0; i < numUiButtons; i++) buttons[i].Debounce();
            static uint8_t slow_poll = 0;
            // HACK: slow encoder response down while selecting mappings
            if (current_mode == &cfg_mode) 
            {
                if ((++slow_poll & 0xF0) == 0) 
                {
                    for (uint8_t i = 0; i < numUiEncoders; i++) encoders[i]->GetDelta();                    
                }
            }
            else 
            {
                for (uint8_t i = 0; i < numUiEncoders; i++) encoders[i]->GetDelta();
            }
        }

        void Process()
        {
            Heartbeat();
            // work through queue of events shovelled out by SysCtrl
        }

        void Light(LightId light, bool on)
        {
            if (light < numUiLights) leds[light].Light(on);
        }

        

    private: 
        static Led leds[numUiLights];
        friend class Button;
        friend class Encoder;
        static Button buttons[numUiButtons];
        static Encoder* encoders[numUiEncoders];        

        friend class UiCtrlState;
        friend class UiCfgState;

        enum UiMode : std::uint8_t
        {
            ControllingTarget = 0,
            ConfiguringSoftTouch,
            numUiModes
        };

        UiStateInterface *current_mode;
        UiCfgState cfg_mode;
        UiCtrlState ctrl_mode;
        
        Ui() : cfg_mode(), ctrl_mode(), current_mode(&ctrl_mode) {}

        ~Ui() {}

        void Heartbeat()
        {
            static uint8_t heartbeat_counter{0};
            if (heartbeat_counter ==    0) Light(BoardRedLed, true);
            if (heartbeat_counter == 0x10) {
                Light(BoardRedLed, false);
            }
            ++heartbeat_counter;
        }

        void ChangeMode(UiMode new_mode)
        {
            switch (new_mode)
            {
                case ControllingTarget:
                    current_mode = &ctrl_mode;
                    printf("mode -> controlling target\r\n");
                    break;
                case ConfiguringSoftTouch:
                    current_mode = &cfg_mode;
                    printf("mode -> configuring soft touch\r\n");
                    break;
                default:
                    break;
            }
        }
    };

}