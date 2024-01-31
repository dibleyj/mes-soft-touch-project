#include "uictrlstate.h"
#include "ui.h"
#include "system.h"

namespace soft_touch 
{

void UiCtrlState::Turn(EncoderId id, int8_t delta)
{
    switch(id)
    {
    case MainEncoder:
        STEventMessage m{UiMgr, SysCtrl, SysCtrlUpdateTargetCtrlVal, delta};
        STEvent rv = SystemController::instance().Post(m);
        (rv == EventMsgRx) ? : printf("Failed to send to SysCtrl\r\n");
        break;

    }
} 

void UiCtrlState::PressAndHold(ButtonId id)
{
    switch(id)
    {
    case BoardSwLH:
        Ui::instance().ChangeMode(Ui::ConfiguringSoftTouch);
        break;
    default:
        break;
    }
}

void UiCtrlState::Release(ButtonId id)
{

}

void UiCtrlState::ReleaseFromHold(ButtonId id)
{

}

void UiCtrlState::Light(LightId id, bool on)
{

}

void UiCtrlState::WriteToDisplay(DisplayId id, uint8_t* bytestring)
{
    // printf("ctrl->wr_display, %u %u\r\n", bytestring[0], bytestring[3]);   
    switch (id)
    {
    case KL46ZSegmentLcd:
        Ui::instance().displays[KL46ZSegmentLcd]->WriteUint8H(STLCD::LH, bytestring[0]);
        Ui::instance().displays[KL46ZSegmentLcd]->WriteUint8H(STLCD::RH, bytestring[3]);
        break;
    default:
        break;
    }
}

}
