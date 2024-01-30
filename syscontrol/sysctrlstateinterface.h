#pragma once
#include "soft_touch.h"

namespace soft_touch 
{

class SysCtrlStateInterface
{
public: 
    SysCtrlStateInterface(){}
    virtual ~SysCtrlStateInterface(){}
    virtual void UpdateControl(STEventMessage m) = 0;
};

}