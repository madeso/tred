#include "tred/input-activemasteraxis.h"
#include <cassert>

#include "tred/input-activeaxis.h"
#include "tred/input-activerangetoaxis.h"


namespace input
{


ActiveMasterAxis::ActiveMasterAxis(InputAction* action, ActiveAxis* a, ActiveRangeToAxis* r)
    : InputActive(action)
    , axis(a)
    , range(r)
{
    assert(axis);
    assert(range);
}


void ActiveMasterAxis::Update(float)
{
    const float axisvalue = axis->GetNormalizedState();
    const float rangevalue = range->GetNormalizedState();

    state_ = rangevalue + axisvalue;
}


}  // namespace input
