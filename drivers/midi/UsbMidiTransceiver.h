#pragma once
#include "soft_touch.h"

namespace soft_touch
{

class UsbMidiTransceiver
{
public:
    static UsbMidiTransceiver &
    instance(void)
    {
        static UsbMidiTransceiver m_instance;
        return m_instance;
    }

    void Init()
    {}

    void Process()
    {
        ProcessEventsToHost();
    } 

    STEvent Post(STEventMessage msg)
    {
        // check queue can receive messages, reject if so
        if (to_host.full())
        {
            return EventMsgRxFail;
        }
        to_host.push(msg);    
        return EventMsgRx;
    }

private:
    UsbMidiTransceiver() {}

    ~UsbMidiTransceiver() {}

    void ProcessEventsToHost()
    {
        if (to_host.empty()) return; 
        STEventMessage rx = {STNode::None, STNode::None, STEvent::Invalid, 0};
        to_host.pop(rx);
        switch (rx.e)
        {
        case UsbMidiCcMsgToHost:
            if (rx.src == SysCtrl) SendMidiCcMsgToHost(rx.v);
            break;
        default:
            break;
        }
    }

    void SendMidiCcMsgToHost(int32_t value)
    {
        // Unpack MIDI chan, MIDI CC num and target value
        // Assumes 7b target value
        uint8_t midi_chan{0}, midi_cc{0}, target_value{0}; 
        uint32_t data = static_cast<uint32_t>(value);
        midi_chan       = (data & 0xFF000000) >> 24;
        midi_cc         = (data & 0x00FF0000) >> 16;
        target_value    = (data & 0x000000FF);
        // printf("midi cc heading out, %x / %x / %x\r\n", midi_chan, midi_cc, target_value);
        midi_txrx.write(MIDIMessage::ControlChange(
                            midi_cc, target_value, midi_chan));
    }

    USBMIDI midi_txrx; 

    static const uint8_t k_event_buffer_size{8};

    CircularBuffer<STEventMessage,k_event_buffer_size> to_host; 
};

}