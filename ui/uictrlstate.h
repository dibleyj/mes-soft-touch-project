#pragma once
#include "uistateinterface.h"

namespace soft_touch
{

class UiCtrlState : public UiStateInterface
{
public:
    UiCtrlState() : UiStateInterface() {}

    void Turn(EncoderId id, int8_t delta);

    void PressAndHold(ButtonId id);
    
    void Release(ButtonId id);
    
    void ReleaseFromHold(ButtonId id);
    
    void Light(LightId id, bool on);
private:

};

}