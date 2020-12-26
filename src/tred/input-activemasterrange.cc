#include "tred/input-activemasterrange.h"
#include <cassert>

#include "tred/input-activerange.h"
#include "tred/input-activeaxistorange.h"


namespace input
{

    
ActiveMasterRange::ActiveMasterRange(InputAction* action, ActiveRange* r, ActiveAxisToRange* a)
    : InputActive(action)
    , range(r)
    , axis(a)
{
    assert(range);
    assert(axis);
}

void ActiveMasterRange::Update(float)
{
    const float axisvalue = axis->GetNormalizedState();
    const float rangevalue = range->GetNormalizedState();

    state_ = rangevalue + axisvalue;
}


}  // namespace input
