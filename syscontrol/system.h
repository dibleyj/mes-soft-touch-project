#pragma once
#include "soft_touch.h"
#include "sysctrlmappinginterface.h"
#include "sysctrlmapping.h"

namespace soft_touch 
{

class SystemController 
{
public:
    static SystemController & 
    instance()
    {
        static SystemController m_instance;
        
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
        : mapping(mappings)
    {

    }

    ~SystemController() {}

    void ProcessRxEvents()
    {
        if (rx_events.empty()) return; 
        STEventMessage rx = {STNode::None, STNode::None, STEvent::Invalid, 0};
        rx_events.pop(rx);
        mapping->UpdateMapping(rx);
        return;
    } 

    SysCtrlMappingInterface *mapping; 

    static const uint8_t k_num_mappings{8};

    static SysCtrlMapping mappings[k_num_mappings];

    static const uint8_t k_event_buffer_size{8};

    CircularBuffer<STEventMessage,k_event_buffer_size> rx_events; 
};

}