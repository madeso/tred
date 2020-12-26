#include "tred/input-activerange.h"
#include <cassert>
#include <string>
#include "fmt/format.h"
#include "tred/input-bind.h"

#include "tred/input-action.h"


namespace input
{
ActiveRange::ActiveRange(InputAction* action, Bind* r)
    : InputActive(action)
    , range(r)
{
    assert(range);

    if (range->type != BindType::RANGE)
    {
        throw fmt::format("bound type for {} is not a range, is {}", action->name, range->type);
    }
}

void ActiveRange::Update(float)
{
    assert(range);
    state_ = range->value;
}

}  // namespace input
