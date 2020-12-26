#include "tred/input-hataxis.h"

#include <cassert>


namespace input
{


HatAxis::HatAxis(int h, Axis a)
    : hat(h)
    , axis(a)
{
}


bool HatAxis::operator<(const HatAxis& rhs) const
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
