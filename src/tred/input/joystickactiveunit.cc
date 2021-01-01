#include "tred/input/joystickactiveunit.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"


namespace input
{
JoystickActiveUnit::JoystickActiveUnit
    (
        int,
        InputDirector* d, Converter* converter,
        const std::vector<BindDef<int>>& a,
        const std::vector<BindDef<int>>& b,
        const std::vector<BindDef<HatAxis>>& h
    )
    // : joystick_(joystick)
    : director(d)
    , axes(a, converter)
    , buttons(b, converter)
    , hats(h, converter)
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
    axes.Recieve(reciever);
    buttons.Recieve(reciever);
    hats.Recieve(reciever);
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


}  // namespace input
