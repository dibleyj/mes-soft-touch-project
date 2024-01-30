#pragma once
#include "soft_touch.h"
#include "sysctrlstateinterface.h"

namespace soft_touch 
{

class SysCtrlState : public SysCtrlStateInterface
{
public:
    SysCtrlState(){}

    ~SysCtrlState(){}

    void UpdateControl(STEventMessage m)
    {
        // printf("In SysCtrlState->UpdateControl with event msg %u / %u / %u / %i \r\n",
        //     m.src, m.dst, m.e, m.v);
        switch (m.src)
        {
            case STNode::DevCli:
            default:
            break;
        }
    }
};

}
