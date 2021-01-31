#include "tred/input/activeunit.mouse.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"


namespace input
{


void impl::MouseKeyUnit::RegisterKey(int key)
{
    parent->buttons.Add(FromIndex<MouseButton>(key));
}


float impl::MouseKeyUnit::GetState(int key)
{
    return parent->buttons.GetRaw(FromIndex<MouseButton>(key));
}


void impl::RelativeMouseAxisUnit::RegisterAxis(AxisType type, int target, int axis)
{
    assert(target == 0);
    if(type == AxisType::GeneralAxis)
    {
        parent->relative_axes.Add(FromIndex<Axis>(axis));
    }
    else if(type == AxisType::Wheel)
    {
        parent->wheels.Add(FromIndex<Axis>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
    }
}


float impl::RelativeMouseAxisUnit::GetState(AxisType type, int target, int axis, float)
{
    assert(target == 0);
    if(type == AxisType::GeneralAxis)
    {
        return parent->relative_axes.GetRaw(FromIndex<Axis>(axis));
    }
    else if(type == AxisType::Wheel)
    {
        return parent->wheels.GetRaw(FromIndex<Axis>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
        return 0.0f;
    }
}


void impl::AbsoluteMouseAxisUnit::RegisterAxis(AxisType type, int target, int axis)
{
    assert(target == 0);
    if(type == AxisType::GeneralAxis)
    {
        parent->absolute_axes.Add(FromIndex<Axis>(axis));
    }
    else if(type == AxisType::Wheel)
    {
        // todo(Gustav): implement absolute mouse wheel
        assert(false && "not implemented yet");
    }
    else
    {
        assert(false && "invalid mouse axis");
    }
}


// float impl::AbsoluteMouseAxisUnit::GetState(AxisType type, int target, int axis, float dt)
float impl::AbsoluteMouseAxisUnit::GetState(AxisType type, int target, int axis, float)
{
    assert(target == 0);
    if(type == AxisType::GeneralAxis)
    {
        return parent->absolute_axes.GetRaw(FromIndex<Axis>(axis));
    }
    else if(type == AxisType::Wheel)
    {
        // todo(Gustav): implement absolute mouse wheel
        assert(false && "not implemented yet");
        return 0.0f;
    }
    else
    {
        assert(false && "invalid mouse axis");
        return 0.0f;
    }
}


MouseActiveUnit::MouseActiveUnit(InputDirector* d)
    : director(d)
{
    assert(director);

    director->Add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


MouseActiveUnit::~MouseActiveUnit()
{
    director->Remove(this);
}


KeyUnit* MouseActiveUnit::GetKeyUnit()
{
    return &key_unit;
}

AxisUnit* MouseActiveUnit::GetRelativeAxisUnit()
{
    return &relative_axis_unit;
}

AxisUnit* MouseActiveUnit::GetAbsoluteAxisUnit()
{
    return &absolute_axis_unit;
}



bool MouseActiveUnit::IsConsideredJoystick()
{
    return false;
}


bool MouseActiveUnit::IsDeleteSheduled()
{
    return false;
}


void MouseActiveUnit::OnAxis(const Axis& axis, float relative_state, float absolute_state)
{
    relative_axes.SetRaw(axis, relative_state);
    absolute_axes.SetRaw(axis, absolute_state);
}


void MouseActiveUnit::OnWheel(const Axis& axis, float state)
{
    wheels.SetRaw(axis, state);
}


void MouseActiveUnit::OnButton(MouseButton button, float state)
{
    buttons.SetRaw(button, state);
}


}  // namespace input
