#include "tred/input-mouseactiveunit.h"
#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"
#include "tred/input-commondef.h"


namespace input
{
MouseActiveUnit::MouseActiveUnit(
        const std::vector<std::shared_ptr<TAxisBind<Axis>>>& axis,
        const std::vector<std::shared_ptr<TRangeBind<MouseButton>>>& buttons,
        InputDirector* director)
    : director_(director)
    , actions_(ConvertToBindMap<TAxisBind<Axis>, Axis>(axis))
    , buttons_(
              ConvertToBindMap<TRangeBind<MouseButton>, MouseButton>(buttons))
{
    assert(director_);

    director_->Add(this);
}

void MouseActiveUnit::OnAxis(const Axis& key, float state)
{
    auto res = actions_.find(key);
    if (res != actions_.end())
    {
        TransformAndSetBindValue(res->second, state);
    }
}

void MouseActiveUnit::OnButton(MouseButton key, float state)
{
    auto res = buttons_.find(key);
    if (res != buttons_.end())
    {
        TransformAndSetBindValue(res->second, state);
    }
}

MouseActiveUnit::~MouseActiveUnit()
{
    director_->Remove(this);
}

void MouseActiveUnit::Rumble()
{
}

}  // namespace input
