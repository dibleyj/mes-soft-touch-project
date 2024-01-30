#pragma once
#include "soft_touch.h"
#include "sysctrlmappinginterface.h"

namespace soft_touch 
{

class SysCtrlMapping : public SysCtrlMappingInterface
{
public:
    SysCtrlMapping(uint8_t index, uint8_t target_chan, uint8_t target_cc_num, uint8_t value) 
        : SysCtrlMappingInterface(index, target_chan, target_cc_num)
        , idx(index)
        , target_value(value)
    {}

    ~SysCtrlMapping(){}

    void UpdateMapping(STEventMessage m)
    {
        switch (m.src)
        {
            case STNode::UsbMidi:
                // Not implemented yet: Feedback from host (user changed target value directly)
                break;
            case STNode::UiMgr:
                // User moved controlling encoder   
                UpdateMappingTargetValue(m.v);
                break;
            case STNode::DevCli:
                // Developer updated mapping from Console
                break;
            default:
                break;
        }
    }

    enum SysCtrlTargetCc
    {
        // some interesting target controlchange opcodes
        MidiCcVolume = 7,
        MidiCcExpression = 11,
        MidiCcHold = 64,
        MidiCcSostenuto = 66,
        MidiCc102 = 102, // 102-119 conveniently undefined
    };

private:
    uint8_t target_value, idx;
    
    void UpdateMappingTargetValue(int8_t delta)
    {
        uint8_t nv = ClampEncoderInt8Reading(target_value, delta);
        if (nv != target_value) target_value = nv;
        // TODO: Send to UsbMidi
        printf("mapping %u is now -> %u\r\n", idx, target_value);
    }

    int8_t ClampEncoderInt8Reading(int8_t cv, int8_t d)
    {
        // I don't much want to talk about how long it took me to get this right
        return (cv + d) < 0 ? 0 : 
                    (INT8_MAX < (cv + d) ? INT8_MAX : (cv + d));
    }

    int16_t ClampEncoderInt16Reading(int16_t cv, int16_t d)
    {
        return (cv + d) < 0 ? 0 : 
                    (INT16_MAX < (cv + d) ? INT16_MAX : (cv + d));
    }
};

}
