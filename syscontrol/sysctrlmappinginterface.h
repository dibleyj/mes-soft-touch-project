#pragma once
#include "soft_touch.h"

namespace soft_touch 
{

class SysCtrlMappingInterface
{
    // Apply 'State' design pattern 
public: 
    SysCtrlMappingInterface(uint8_t index, uint8_t target_chan, uint8_t target_cc){}
    virtual ~SysCtrlMappingInterface(){}
    virtual void UpdateMapping(STEventMessage m) = 0;

    
};

}