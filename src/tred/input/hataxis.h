#pragma once

#include "tred/input/axis.h"


namespace input
{


struct hat_and_xy_axis
{
    hat_and_xy_axis(int hat, xy_axis axis);

    bool operator<(const hat_and_xy_axis& rhs) const;

    int hat;
    xy_axis axis;
};


}
