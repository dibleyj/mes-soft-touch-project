
#include "sysctrlmapping.h"

namespace soft_touch
{

    void SysCtrlMapping::UpdateMappingTargetValue(int8_t delta)
    {
        uint8_t nv = ClampEncoderInt8Reading(target_value, delta);
        if (nv != target_value) target_value = nv;
        // printf("mapping %u is now -> %u\r\n", index, target_value);
        // TODO: Send to UsbMidi
        STEventMessage midi_m{SysCtrl, UsbMidi, UsbMidiCcMsgToHost, 0};
        midi_m.v = ((target_chan << 24) | (target_cc << 16) | target_value );
        // printf("midi_m.v = %X\r\n", midi_m.v);
        STEvent rv = UsbMidiTransceiver::instance().Post(midi_m);
        (rv == EventMsgRx) ? : printf("CtrlMapping failed to send to UsbMidi\r\n");
        // TODO: Send to Ui
        STEventMessage ui_m{SysCtrl, UiMgr, UiDisplayUpdate, 0};
        ui_m.v = (((1 + index) << 24) | (' ' << 16) | midi_m.v);
        rv = Ui::instance().Post(ui_m);
        (rv == EventMsgRx) ? : printf("CtrlMapping failed to send to Ui\r\n");
    }
}