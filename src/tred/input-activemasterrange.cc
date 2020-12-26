#include "tred/input-activemasterrange.h"
#include <cassert>

#include "tred/input-activerange.h"
#include "tred/input-activeaxistorange.h"


namespace input
{
ActiveMasterRange::ActiveMasterRange(InputAction* action, ActiveRange* range, ActiveAxisToRange* axis)
    : InputActive(action)
    , range_(range)
    , axis_(axis)
{
    assert(range_);
    assert(axis_);
}

void ActiveMasterRange::Update(float)
{
    const float axisvalue = axis_->state();
    const float rangevalue = range_->state();

    return set_state(rangevalue + axisvalue);
}

}  // namespace input
