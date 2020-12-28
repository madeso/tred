#include "tred/input-mouseactiveunit.h"

#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"
#include "tred/input-commondef.h"


namespace input
{


MouseActiveUnit::MouseActiveUnit
    (
        const std::vector<std::shared_ptr<TAxisBind<Axis>>>& axis,
        const std::vector<std::shared_ptr<TRangeBind<MouseButton>>>& buttons,
        InputDirector* d
    )
    : director(d)
    , actions(ConvertToBindMap<TAxisBind<Axis>, Axis>(axis))
    , buttons(ConvertToBindMap<TRangeBind<MouseButton>, MouseButton>(buttons))
{
    assert(director);

    director->Add(this);
}


MouseActiveUnit::~MouseActiveUnit()
{
    director->Remove(this);
}


void MouseActiveUnit::OnAxis(const Axis& key, float state)
{
    auto res = actions.find(key);
    if (res != actions.end())
    {
        TransformAndSetBindValue(res->second, state);
    }
}


void MouseActiveUnit::OnButton(MouseButton key, float state)
{
    auto res = buttons.find(key);
    if (res != buttons.end())
    {
        TransformAndSetBindValue(res->second, state);
    }
}


}  // namespace input
