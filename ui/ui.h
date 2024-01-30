#pragma once
#include "soft_touch.h"
#include "QEI.h"
#include "STLCD.h"
#include "system.h"
#include "uistateinterface.h"

using namespace std;

namespace soft_touch {

    class Ui {
    public: 
        enum UiLight
        {
            UiLight01,
            UiLight02,
            numUiLights
        };

        static Ui & 
        instance()
        {
            static Ui m_instance;
            return m_instance;
        }

        void Init()
        {

        }

        void Poll()
        {
            pulses = enc1.getPulses();
        }

        void Process()
        {
            Light(UiLight01, true); // heartbeat
            UpdateEncoder();
            Light(UiLight01, false); // heartbeat
        }

        void Light(UiLight light, bool on)
        {
            switch (light)
            {
            case UiLight01:
                on ? (led1 = 0) : (led1 = 1);
                break;
            case UiLight02:
            default:
                break;
            }            
        }


    private: 
        DigitalOut led1;
        QEI enc1;
        int32_t pulses{0};
        int32_t last_pulses{0};
        uint8_t whatever{0};
        int8_t controller_val{0};
        
        Ui() :
            led1(LED1),
            enc1(D2, D3, NC, 24, QEI::X2_ENCODING) {}

        ~Ui() {}

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
                STEventMessage m = {STNode::Ui, STNode::SysCtrl, STEvent::UiEncoderTurn, delta};
                STEvent rv = SystemController::instance().Post(m);
                (rv == EventMsgRx) ? printf ("Sent to SysCtrl OK!\r\n") : printf("Failed to send to SysCtrl\r\n");

            }
        }

        int8_t ClampEncoderInt8Reading(int8_t cv, int8_t d)
        {
            return (cv + d) < 0 ? 0 : (INT8_MAX < (cv + d) ? INT8_MAX : (cv + d));
        }
    };

}