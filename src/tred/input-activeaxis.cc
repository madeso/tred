#include "tred/input-activeaxis.h"
#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-bind.h"
#include "tred/input-action.h"


namespace input
{
ActiveAxis::ActiveAxis(InputAction* action, Bind* a)
    : InputActive(action)
    , axis(a)
{
    assert(axis);

    if (axis->type != BindType::AXIS)
    {
        const std::string error = fmt::format("bound type for {} is not a axis, is {}", action->name, axis->type);
        throw error;
    }
}

void ActiveAxis::Update(float)
{
    assert(axis);
    state_ = axis->value;
}

}  // namespace input
