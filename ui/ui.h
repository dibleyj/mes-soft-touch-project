#pragma once
#include "soft_touch.h"
#include "QEI.h"
#include "STLCD.h"
#include "led.h"
#include "button.h"
// #include "system.h"
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
            // (because there are only 8 of them -- the QEI library does X2 
            // which "feels nice" for regular target control, but the basic
            // stepping rate is too brisk for fine control... so we just toss
            // some readings away)
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

        STEvent Post(STEventMessage msg)
        {
            // check queue can receive messages, reject if so
            if (from_sysctrl.full())
            {
                return EventMsgRxFail;
            }
            from_sysctrl.push(msg);    
            return EventMsgRx;
        }

        void Process()
        {
            Heartbeat();
            // work through queue of events shovelled out by SysCtrl
            ProcessEventsFromSysCtrl();
        }

        void ProcessEventsFromSysCtrl()
        {
            if (from_sysctrl.empty()) return; 
            // printf("ProcessEventsFromSysCtrl, from_sysctrl = %u", from_sysctrl.size());
            STEventMessage rx = {STNode::None, STNode::None, STEvent::Invalid, 0};
            from_sysctrl.pop(rx);
            switch (rx.e)
            {
            case UiDisplayUpdate:
                if (rx.src == SysCtrl) SendDisplayUpdate(rx.v);
                break;
            default:
                break;
            }
        }

        void Light(LightId light, bool on)
        {
            if (light < numUiLights) leds[light].Light(on);
        }

        void SendDisplayUpdate(int32_t v)
        {
            uint32_t uv = static_cast<uint32_t>(v);
            uint8_t displaybuf[]{0,0,0,0};
            displaybuf[0] = (uv & 0xFF000000) >> 24;
            displaybuf[1] = (uv & 0x00FF0000) >> 16;
            displaybuf[2] = (uv & 0x0000FF00) >> 8;
            displaybuf[3] = (uv & 0x000000FF);
            printf("SendDispUpdate: %u, %u\r\n", displaybuf[0], displaybuf[3]);
            current_mode->WriteToDisplay(KL46ZSegmentLcd, displaybuf);
        }
        

    private: 
        static Led leds[numUiLights];
        friend class Button;
        friend class Encoder;
        friend class STLCD;
        static Button buttons[numUiButtons];
        static Encoder* encoders[numUiEncoders];   
        static STLCD* displays[numUiDisplays];     

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

        static const uint8_t k_event_buffer_size{8};

        CircularBuffer<STEventMessage,k_event_buffer_size> from_sysctrl; 

        Ui() : cfg_mode(), ctrl_mode(), current_mode(&ctrl_mode) {}

        ~Ui() {}
    
        void Heartbeat()
        {
            static uint8_t heartbeat_counter{0};
            if (heartbeat_counter ==    0) Light(BoardGreenLed, true);
            if (heartbeat_counter == 0x10) {
                Light(BoardGreenLed, false);
            }
            ++heartbeat_counter;
        }

        void ChangeMode(UiMode new_mode)
        {
            switch (new_mode)
            {
                case ControllingTarget:
                    current_mode = &ctrl_mode;
                    // printf("mode -> controlling target\r\n");
                    break;
                case ConfiguringSoftTouch:
                    current_mode = &cfg_mode;
                    // printf("mode -> configuring soft touch\r\n");
                    break;
                default:
                    break;
            }
        }
    };

}