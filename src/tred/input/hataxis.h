#pragma once

#include "tred/input/axis.h"


namespace input
{


struct HatAxis
{
    HatAxis(int hat, Axis axis);

    bool operator<(const HatAxis& rhs) const;

    int hat;
    Axis axis;
};


}  // namespace input
