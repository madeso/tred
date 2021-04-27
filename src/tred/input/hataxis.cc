#include "tred/input/hataxis.h"

#include <cassert>


namespace input
{


hat_and_xy_axis::hat_and_xy_axis(int h, xy_axis a)
    : hat(h)
    , axis(a)
{
}


bool hat_and_xy_axis::operator<(const hat_and_xy_axis& rhs) const
{
    if (axis == rhs.axis)
    {
        return hat < rhs.hat;
    }
    else
    {
        return axis < rhs.axis;
    }
}


}  // namespace input
