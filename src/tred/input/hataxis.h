#pragma once

#include "tred/input/axis.h"


namespace input
{


struct HatAndAxis2
{
    HatAndAxis2(int hat, Axis2 axis);

    bool operator<(const HatAndAxis2& rhs) const;

    int hat;
    Axis2 axis;
};


}
