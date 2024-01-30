#pragma once
#include "soft_touch.h"
#include "sysctrlmappinginterface.h"

namespace soft_touch 
{

class SysCtrlMapping : public SysCtrlMappingInterface
{
public:
    SysCtrlMapping(uint8_t index, uint8_t target_chan, uint8_t target_cc_num) 
        : SysCtrlMappingInterface(index, target_chan, target_cc_num)
    {}

    ~SysCtrlMapping(){}

    void UpdateMapping(STEventMessage m)
    {
        switch (m.src)
        {
            case STNode::UsbMidi:
                // Not implemented yet: Feedback from host (user changed target value directly)
                break;
            case STNode::Ui:
                // User moved controlling encoder
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

};

}
