#include "tred/input/activeunit.gamecontroller.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::GamecontrollerKeyUnit::RegisterKey(int key)
{
    parent->buttons.Add(FromIndex<GamecontrollerButton>(key));
}


float impl::GamecontrollerKeyUnit::GetState(int key)
{
    return parent->buttons.GetRaw(FromIndex<GamecontrollerButton>(key));
}


impl::GamecontrollerAxisUnit::GamecontrollerAxisUnit(bool ir)
    : is_relative(ir)
{
}


void impl::GamecontrollerAxisUnit::RegisterAxis(AxisType type, int target, int axis)
{
    switch(type)
    {
    case AxisType::GeneralAxis:
        assert(target == 0);
        parent->axes.Add(FromIndex<GamecontrollerAxis>(axis));
        break;
    default:
        assert(false && "invalid type");
    }
}


float impl::GamecontrollerAxisUnit::GetState(AxisType type, int target, int axis, float dt)
{
    switch(type)
    {
    case AxisType::GeneralAxis:
        assert(target == 0);
        return SmoothAxis(parent->axes.GetRaw(FromIndex<GamecontrollerAxis>(axis))) * dt;
    default:
        assert(false && "invalid type");
        return 0.0f;
    }
}


GamecontrollerActiveUnit::GamecontrollerActiveUnit(JoystickId j, InputDirector* d)
    : joystick(j)
    , director(d)
    , sheduled_delete(false)
    , relative_axis_unit(true)
    , absolute_axis_unit(false)
{
    assert(director);
    director->Add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


GamecontrollerActiveUnit::~GamecontrollerActiveUnit()
{
    director->Remove(this);
}


KeyUnit* GamecontrollerActiveUnit::GetKeyUnit()
{
    return &key_unit;
}


AxisUnit* GamecontrollerActiveUnit::GetRelativeAxisUnit()
{
    return &relative_axis_unit;
}


AxisUnit* GamecontrollerActiveUnit::GetAbsoluteAxisUnit()
{
    return &absolute_axis_unit;
}


bool GamecontrollerActiveUnit::IsConsideredJoystick()
{
    return true;
}


bool GamecontrollerActiveUnit::IsDeleteSheduled()
{
    return sheduled_delete;
}


void GamecontrollerActiveUnit::OnAxis(GamecontrollerAxis axis, float state)
{
    axes.SetRaw(axis, state);
}


void GamecontrollerActiveUnit::OnButton(GamecontrollerButton button, float state)
{
    buttons.SetRaw(button, SmoothRange(state));
}


}  // namespace input
