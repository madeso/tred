#include "tred/input-activerangetoaxis.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-bind.h"
#include "tred/input-action.h"


namespace input
{

ActiveRangeToAxis::ActiveRangeToAxis(InputAction* action, Bind* positive, Bind* negative)
    : InputActive(action)
    , positive_(positive)
    , negative_(negative)
{
    assert(positive_);
    assert(negative_);

    if (positive_->type != BindType::RANGE)
    {
        const std::string error = fmt::format("bound type for positive {} is not a range, is {}", action->name, positive_->type);
        throw error;
    }

    if (negative_->type != BindType::RANGE)
    {
        const std::string error = fmt::format("bound type for negative {} is not a range, is {}", action->name, negative_->type);
        throw error;
    }
}


void ActiveRangeToAxis::Update(float)
{
    assert(positive_);
    assert(negative_);
    state_ = positive_->value - negative_->value;
}


}  // namespace input
