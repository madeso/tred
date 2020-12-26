#include "tred/input-activemasteraxis.h"
#include <cassert>

#include "tred/input-activeaxis.h"
#include "tred/input-activerangetoaxis.h"


namespace input
{
ActiveMasterAxis::ActiveMasterAxis(InputAction* action, ActiveAxis* axis, ActiveRangeToAxis* range)
    : InputActive(action)
    , axis_(axis)
    , range_(range)
{
    assert(axis_);
    assert(range_);
}

void ActiveMasterAxis::Update(float)
{
    const float axisvalue = axis_->state();
    const float rangevalue = range_->state();

    return set_state(rangevalue + axisvalue);
}

}  // namespace input
