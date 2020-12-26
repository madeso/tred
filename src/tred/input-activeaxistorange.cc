#include "tred/input-activeaxistorange.h"
#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-bind.h"
#include "tred/input-action.h"


namespace input
{
ActiveAxisToRange::ActiveAxisToRange(InputAction* action, Bind* a)
    : InputActive(action)
    , axis(a)
{
    assert(axis);

    if (axis->type != BindType::AXIS)
    {
        throw fmt::format("bound type for axis {} is not a axis, is {}", action->name, axis->type);
    }
}

void ActiveAxisToRange::Update(float)
{
    assert(axis);
    float v = axis->value;
    
    if (v < 0)
    {
        v = 0;
    }

    state_ = v;
}

}  // namespace input
