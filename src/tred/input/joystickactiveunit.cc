#include "tred/input/joystickactiveunit.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"


namespace input
{


JoystickActiveUnit::JoystickActiveUnit
    (
        JoystickId j,
        InputDirector* d, Converter* converter,
        const std::vector<BindDef<int>>& a,
        const std::vector<BindDef<int>>& b,
        const std::vector<BindDef<HatAxis>>& h,
        const std::vector<BindDef<HatAxis>>& ba
    )
    : joystick(j)
    , director(d)
    , axes(a, converter)
    , buttons(b, converter)
    , hats(h, converter)
    , balls(ba, converter)
{
    assert(director);
    director->Add(this);
}


JoystickActiveUnit::~JoystickActiveUnit()
{
    director->Remove(this);
}


bool JoystickActiveUnit::IsConsideredJoystick()
{
    return true;
}


void JoystickActiveUnit::OnAxis(int axis, float state)
{
    axes.SetRaw(axis, state);
}


void JoystickActiveUnit::OnButton(int button, float state)
{
    buttons.SetRaw(button, state);
}


void JoystickActiveUnit::OnHat(const HatAxis& hatAxis, float state)
{
    hats.SetRaw(hatAxis, state);
}


void JoystickActiveUnit::OnBall(const HatAxis& hatAxis, float state)
{
    balls.SetRaw(hatAxis, state);
}


}  // namespace input
