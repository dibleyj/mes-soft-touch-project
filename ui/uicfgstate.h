#pragma once
#include "uistateinterface.h"

namespace soft_touch
{

class UiCfgState : public UiStateInterface
{
public: 
    UiCfgState() 
        : UiStateInterface() {}
    
    void Turn(EncoderId id, int8_t delta);

    void PressAndHold(ButtonId id);
    
    void Release(ButtonId id);
    
    void ReleaseFromHold(ButtonId id);
    
    void Light(LightId id, bool on);

    void WriteToDisplay(DisplayId id, uint8_t* bytestring);
private: 
};

}