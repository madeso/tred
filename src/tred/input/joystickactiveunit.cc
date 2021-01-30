#include "tred/input/joystickactiveunit.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::JoystickKeyUnit::RegisterKey(int key)
{
    parent->buttons.Add(key);
}


float impl::JoystickKeyUnit::GetState(int key)
{
    return parent->buttons.GetRaw(key);
}


impl::JoystickAxisUnit::JoystickAxisUnit(bool ir)
    : is_relative(ir)
{
}


void impl::JoystickAxisUnit::RegisterAxis(AxisType type, int target, int axis)
{
    switch(type)
    {
    case AxisType::GeneralAxis:
        assert(target == 0);
        parent->axes.Add(axis);
        break;
    case AxisType::Hat:
        parent->hats.Add({target, FromIndex<Axis>(axis)});
        break;
    case AxisType::Ball:
        parent->balls.Add({target, FromIndex<Axis>(axis)});
        break;
    default:
        assert(false && "invalid type");
    }
}


float impl::JoystickAxisUnit::GetState(AxisType type, int target, int axis, float dt)
{
    switch(type)
    {
    case AxisType::GeneralAxis:
        assert(target == 0);
        return SmoothAxis(parent->axes.GetRaw(axis)) * dt;
    case AxisType::Hat:
        return parent->hats.GetRaw({target, FromIndex<Axis>(axis)}) * dt;
    case AxisType::Ball:
        return parent->balls.GetRaw({target, FromIndex<Axis>(axis)}) * dt;
    default:
        assert(false && "invalid type");
    }
}


JoystickActiveUnit::JoystickActiveUnit(JoystickId j, InputDirector* d)
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


JoystickActiveUnit::~JoystickActiveUnit()
{
    director->Remove(this);
}


KeyUnit* JoystickActiveUnit::GetKeyUnit()
{
    return &key_unit;
}


AxisUnit* JoystickActiveUnit::GetRelativeAxisUnit()
{
    return &relative_axis_unit;
}


AxisUnit* JoystickActiveUnit::GetAbsoluteAxisUnit()
{
    return &absolute_axis_unit;
}


bool JoystickActiveUnit::IsConsideredJoystick()
{
    return true;
}


bool JoystickActiveUnit::IsDeleteSheduled()
{
    return sheduled_delete;
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
