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
        switch (rx.e)
        {
        case SysCtrlLoadMapping:
            if ((rx.src == UiMgr) || (rx.src == UsbMidi)) LoadNewMapping(rx.v);
            break;
        case SysCtrlUpdateTargetCtrlVal:
            if ((rx.src == UiMgr) || (rx.src == UsbMidi)) mapping->UpdateMapping(rx);
            break;
        default:
            break;
        }

        return;
    } 

    void LoadNewMapping(int8_t delta)
    {
        uint8_t new_map_idx{0};
        new_map_idx = ((current_map_idx + delta) < 0) 
                        ? 0 
                        : (k_num_mappings <= (current_map_idx + delta)) 
                            ? (k_num_mappings - 1) 
                            : (current_map_idx + delta);
        current_map_idx = new_map_idx;
        mapping = &(mappings[current_map_idx]);
        printf("new mapping: %u\r\n", current_map_idx);
    }

    SysCtrlMappingInterface *mapping; 

    uint8_t current_map_idx{0}; 

    static const uint8_t k_num_mappings{8};

    static SysCtrlMapping mappings[k_num_mappings];

    static const uint8_t k_event_buffer_size{8};

    CircularBuffer<STEventMessage,k_event_buffer_size> rx_events; 
};

}