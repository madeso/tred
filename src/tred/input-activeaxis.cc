#include "tred/input-activeaxis.h"
#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-bind.h"
#include "tred/input-action.h"


namespace input
{
ActiveAxis::ActiveAxis(InputAction* action, Bind* axis)
    : InputActive(action)
    , axis_(axis)
{
    assert(axis_);

    if (axis_->type() != BindType::AXIS)
    {
        const std::string error = fmt::format("bound type for {} is not a axis, is {}", action->name(), axis_->type());
        throw error;
    }
}

void ActiveAxis::Update(float)
{
    assert(axis_);
    set_state(axis_->value());
}

}  // namespace input
