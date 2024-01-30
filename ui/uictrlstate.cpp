#include "uictrlstate.h"
#include "ui.h"

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

}
