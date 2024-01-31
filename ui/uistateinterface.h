#pragma once
#include "soft_touch.h"
#include "button.h"
#include "encoder.h"
#include "led.h"

namespace soft_touch 
{

class UiStateInterface
{
public: 
    UiStateInterface(){}
    virtual ~UiStateInterface(){}
    virtual void Turn(EncoderId id, int8_t delta) = 0;
    virtual void PressAndHold(ButtonId id) = 0;
    virtual void Release(ButtonId id) = 0;
    virtual void ReleaseFromHold(ButtonId id) = 0;
    virtual void Light(LightId id, bool on) = 0;
    virtual void WriteToDisplay(DisplayId id, uint8_t* bytestring) = 0;
};

}