#pragma once
#include "soft_touch.h"
#include "sysctrlstateinterface.h"
#include "sysctrlstate.h"
#include "syscfgstate.h"

namespace soft_touch 
{

class SystemController 
{
public:
    static SystemController & 
    instance()
    {
        static SystemController m_instance;
        m_instance.state = &ctrl_state;
        return m_instance;
    }

    void Init()
    {

    }

    void Process()
    {
        ProcessRxEvents();
    }

    STEvent Post(STEventMessage msg)
    {
        // check queue can receive messages, reject if so
        if (rx_events.full())
        {
            return EventMsgRxFail;
        }
        rx_events.push(msg);    
        return EventMsgRx;
    }

private: 
    SystemController() 
    {}

    ~SystemController() {}

    void ProcessRxEvents()
    {
        if (rx_events.empty()) return; 
        // printf("SysCtrl: rx queue: %u\r\n", rx_events.size());
        STEventMessage rx = {STNode::None, STNode::None, STEvent::Invalid, 0};
        rx_events.pop(rx);
        state->UpdateControl(rx);
        return;
    } 

    SysCtrlStateInterface *state; 

    static SysCtrlState ctrl_state;

    static SysCfgState cfg_state;

    static const uint8_t k_event_buffer_size{16};

    CircularBuffer<STEventMessage,k_event_buffer_size> rx_events; 
};

}