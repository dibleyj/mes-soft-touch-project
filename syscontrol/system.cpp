#include "system.h"

namespace soft_touch 
{
    SysCtrlMapping SystemController::mappings[k_num_mappings] =
    {
        SysCtrlMapping(0, 0, SysCtrlMapping::MidiCc102, 63),
        SysCtrlMapping(1, 0, SysCtrlMapping::MidiCc102 + 1, 63),
        SysCtrlMapping(2, 0, SysCtrlMapping::MidiCc102 + 2, 63),
        SysCtrlMapping(3, 0, SysCtrlMapping::MidiCc102 + 3, 63),
        SysCtrlMapping(4, 0, SysCtrlMapping::MidiCc102 + 4, 63),
        SysCtrlMapping(5, 0, SysCtrlMapping::MidiCc102 + 5, 63),
        SysCtrlMapping(6, 0, SysCtrlMapping::MidiCc102 + 6, 63),
        SysCtrlMapping(7, 0, SysCtrlMapping::MidiCc102 + 7, 63),
    }; 
}