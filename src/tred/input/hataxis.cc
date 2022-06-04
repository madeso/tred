#include "tred/input/hataxis.h"

#include "tred/assert.h"


namespace input
{


HatAndAxis2::HatAndAxis2(int h, Axis2 a)
    : hat(h)
    , axis(a)
{
}


bool HatAndAxis2::operator==(const HatAndAxis2& rhs) const
{
    return axis == rhs.axis && hat < rhs.hat;
}


}  // namespace input
