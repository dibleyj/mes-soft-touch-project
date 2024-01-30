#pragma once
#include "soft_touch.h"
#include "sysctrlstateinterface.h"

namespace soft_touch 
{

class SysCfgState : public SysCtrlStateInterface
{
public:
    SysCfgState(){}

    ~SysCfgState(){}

    void UpdateControl(STEventMessage m)
    {
        ;
    }
};

}