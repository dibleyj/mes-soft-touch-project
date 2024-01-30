#include "system.h"

namespace soft_touch 
{
    SysCtrlMapping SystemController::mappings[k_num_mappings] =
    {
        SysCtrlMapping(0, 0, SysCtrlMapping::MidiCc102),
        SysCtrlMapping(1, 0, SysCtrlMapping::MidiCc102 + 1),
        SysCtrlMapping(2, 0, SysCtrlMapping::MidiCc102 + 2),
        SysCtrlMapping(3, 0, SysCtrlMapping::MidiCc102 + 3),
        SysCtrlMapping(4, 0, SysCtrlMapping::MidiCc102 + 4),
        SysCtrlMapping(5, 0, SysCtrlMapping::MidiCc102 + 5),
        SysCtrlMapping(6, 0, SysCtrlMapping::MidiCc102 + 6),
        SysCtrlMapping(7, 0, SysCtrlMapping::MidiCc102 + 7),
    }; 
}