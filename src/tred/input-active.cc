#include "tred/input-active.h"
#include <cassert>

#include "tred/input-action.h"


namespace input
{
float KeepWithin(float mi, float v, float ma)
{
    if (v < mi)
    {
        return mi;
    }
    else if (v > ma)
    {
        return ma;
    }
    else
        return v;
}

InputActive::InputActive(InputAction* action)
    : action(action)
    , state_(0.0f)
{
}

float InputActive::GetNormalizedState() const
{
    float value = state_;

    switch (action->range)
    {
    case Range::Infinite:
        /* do nothing */
        break;
    case Range::WithinZeroToOne:
        value = KeepWithin(0.0f, value, 1.0f);
        break;
    case Range::WithinNegativeOneToPositiveOne:
        value = KeepWithin(-1.0f, value, 1.0f);
        break;
    default:
        assert(false && "Using invalid value");
        break;
    }

    return value;
}

}  // namespace input
