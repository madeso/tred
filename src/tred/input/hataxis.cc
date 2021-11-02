#include "tred/input/hataxis.h"

#include <cassert>


namespace input
{


HatAndAxis2::HatAndAxis2(int h, Axis2 a)
    : hat(h)
    , axis(a)
{
}


bool HatAndAxis2::operator<(const HatAndAxis2& rhs) const
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
