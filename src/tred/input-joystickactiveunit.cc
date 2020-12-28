#include "tred/input-joystickactiveunit.h"
#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"
#include "tred/input-commondef.h"


namespace input
{
JoystickActiveUnit::JoystickActiveUnit
    (
        int,
        InputDirector* d,
        const std::vector<std::shared_ptr<TBind<int>>>& axis,
        const std::vector<std::shared_ptr<TBind<int>>>& buttons,
        const std::vector<std::shared_ptr<TBind<HatAxis>>>& hats
    )
    // : joystick_(joystick)
    : director(d)
    , axes(ConvertToBindMap<TBind<int>, int>(axis))
    , buttons(ConvertToBindMap<TBind<int>, int>(buttons))
    , hats(ConvertToBindMap<TBind<HatAxis>, HatAxis>(hats))
{
    assert(director);
    director->Add(this);
}


JoystickActiveUnit::~JoystickActiveUnit()
{
    director->Remove(this);
}


void JoystickActiveUnit::OnAxis(int axis, float state)
{
    auto found = axes.find(axis);
    if (found != axes.end())
    {
        TransformAndSetBindValue(found->second, state);
    }
}


void JoystickActiveUnit::OnButton(int button, float state)
{
    auto found = buttons.find(button);
    if (found != buttons.end())
    {
        TransformAndSetBindValue(found->second, state);
    }
}


void JoystickActiveUnit::OnHat(const HatAxis& hatAxis, float state)
{
    auto found = hats.find(hatAxis);
    if (found != hats.end())
    {
        TransformAndSetBindValue(found->second, state);
    }
}


}  // namespace input
