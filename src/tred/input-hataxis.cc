#include "tred/input-hataxis.h"

#include <cassert>


namespace input
{


HatAxis::HatAxis(int hat, Axis axis)
    : hat_(hat)
    , axis_(axis)
{
}


Axis HatAxis::axis() const
{
    return axis_;
}


int HatAxis::hat() const
{
    return hat_;
}


bool HatAxis::operator<(const HatAxis& rhs) const
{
    if (axis_ == rhs.axis_)
    {
        return hat_ < rhs.hat_;
    }
    else
    {
        return axis_ < rhs.axis_;
    }
}


}  // namespace input
