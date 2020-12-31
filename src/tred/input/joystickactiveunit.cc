#include "tred/input/joystickactiveunit.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"


namespace input
{
JoystickActiveUnit::JoystickActiveUnit
    (
        int,
        InputDirector* d,
        const std::vector<BindDef<int>>& axis,
        const std::vector<BindDef<int>>& buttons,
        const std::vector<BindDef<HatAxis>>& hats
    )
    // : joystick_(joystick)
    : director(d)
    , axes(ConvertToBindMap(axis))
    , buttons(ConvertToBindMap(buttons))
    , hats(ConvertToBindMap(hats))
{
    assert(director);
    director->Add(this);
}


JoystickActiveUnit::~JoystickActiveUnit()
{
    director->Remove(this);
}


void JoystickActiveUnit::Recieve(ValueReciever* reciever)
{
    CallRecieve(axes, reciever);
    CallRecieve(buttons, reciever);
    CallRecieve(hats, reciever);
}


void JoystickActiveUnit::OnAxis(int axis, float state)
{
    auto found = axes.find(axis);
    if (found != axes.end())
    {
        found->second.SetRawState(state);
    }
}


void JoystickActiveUnit::OnButton(int button, float state)
{
    auto found = buttons.find(button);
    if (found != buttons.end())
    {
        found->second.SetRawState(state);
    }
}


void JoystickActiveUnit::OnHat(const HatAxis& hatAxis, float state)
{
    auto found = hats.find(hatAxis);
    if (found != hats.end())
    {
        found->second.SetRawState(state);
    }
}


}  // namespace input
