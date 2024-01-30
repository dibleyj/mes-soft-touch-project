#pragma once
#include "soft_touch.h"

namespace soft_touch 
{

class UiStateInterface
{
public: 
    UiStateInterface(){}
    virtual ~UiStateInterface(){}
    virtual void Turn( /* TO DO */ ) = 0;
    virtual void Press( /* TO DO */ ) = 0;
};

}