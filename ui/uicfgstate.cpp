#include "uicfgstate.h"
#include "sysctrlmapping.h"
#include "ui.h"

namespace soft_touch
{
    
void UiCfgState::Turn(EncoderId id, int8_t delta)
{
    printf("got into UiCfgState::Turn with delta %i\r\n", delta);
    switch (id)
    {
    case MainEncoder:
        STEventMessage m{UiMgr, SysCtrl, SysCtrlLoadMapping, delta};
        STEvent rv = SystemController::instance().Post(m);
        (rv == EventMsgRx) ? : printf("Failed to send to SysCtrl\r\n");
        break;
    }
}

void UiCfgState::PressAndHold(ButtonId id)
{

}

void UiCfgState::Release(ButtonId id)
{

}

void UiCfgState::ReleaseFromHold(ButtonId id)
{
    switch (id) 
    {
    case BoardSwLH:
        Ui::instance().ChangeMode(Ui::ControllingTarget);
        break;
    default:
        break;
    }
}

void UiCfgState::Light(LightId id, bool on)
{

}


}