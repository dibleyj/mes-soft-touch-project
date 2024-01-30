#pragma once
#include "soft_touch.h"
#include "QEI.h"
#include "STLCD.h"
#include "led.h"
#include "button.h"
#include "system.h"
#include "uistateinterface.h"

using namespace std;

namespace soft_touch {

    class Ui {
    public: 
        enum UiLight
        {
            BoardGreenLed,
            BoardRedLed,
            numUiLights
        };

        enum UiButton 
        {   
            BoardSwLH,
            BoardSwRH,
            EncoderSw,
            numUiButtons
        };

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
            pulses = enc1.getPulses();
        }

        void Process()
        {
            Heartbeat();

            UpdateEncoder();
        }

        void Light(UiLight light, bool on)
        {
            if (light < numUiLights) leds[light].Light(on);
        }


    private: 
        static const uint8_t k_num_encoders{1};
        static Led leds[numUiLights];
        static Button buttons[numUiButtons];
        // Led led1;
        // Button sw1, sw2;
        QEI enc1;
        int32_t pulses{0};
        int32_t last_pulses{0};
        uint8_t whatever{0};
        int8_t controller_val{0};
        
        Ui() :
            // led1(LED1),
            enc1(D2, D3, NC, 24, QEI::X2_ENCODING)
            // sw1(SW1),
            // sw2(SW3) 
            {}

        ~Ui() {}

        void Heartbeat()
        {
            static uint8_t heartbeat_counter{0};
            if (heartbeat_counter ==    0) Light(BoardRedLed, true);
            if (heartbeat_counter == 0x10) {
                Light(BoardRedLed, false);
                for (uint8_t i = 0; i < numUiButtons; i++)
                {
                    if (buttons[i].pressed()) 
                    {
                        printf("button %i pressed!\r\n", i); 
                    }
                    else if (buttons[i].released())
                    {
                        printf("button %i released! \r\n", i);
                    }
                }
            }
            ++heartbeat_counter;
        }

        void UpdateEncoder()
        {
            int8_t delta;
            if (pulses != last_pulses) 
            {
                (pulses > last_pulses) ? (delta = 1) : (delta = -1);                   
                last_pulses = pulses;
                controller_val = ClampEncoderInt8Reading(controller_val, delta);
                STLCD::instance().Write_Msg("a");
                STLCD::instance().Colon(true);
                STLCD::instance().WriteUint8H(STLCD::RH, controller_val);
                STEventMessage m = {STNode::Ui, STNode::SysCtrl, STEvent::SysCtrlUpdateTargetCtrlVal, delta};
                STEvent rv = SystemController::instance().Post(m);
                (rv == EventMsgRx) ? : printf("Failed to send to SysCtrl\r\n");
            }
        }


        int8_t ClampEncoderInt8Reading(int8_t cv, int8_t d)
        {
            return (cv + d) < 0 ? 0 : (INT8_MAX < (cv + d) ? INT8_MAX : (cv + d));
        }
    };

}