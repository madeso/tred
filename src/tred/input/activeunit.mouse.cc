#include "tred/input/activeunit.mouse.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"


namespace input
{


void impl::MouseKeyUnit::register_key(int key)
{
    parent->buttons.add(from_index<MouseButton>(key));
}


float impl::MouseKeyUnit::get_state(int key)
{
    return parent->buttons.get_raw(from_index<MouseButton>(key));
}


void impl::RelativeMouseAxisUnit::register_axis(AxisType type, [[maybe_unused]]int target, int axis)
{
    assert(target == 0);
    if(type == AxisType::general_axis)
    {
        parent->relative_axes.add(from_index<Axis2>(axis));
    }
    else if(type == AxisType::wheel)
    {
        parent->wheels.add(from_index<Axis2>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
    }
}


float impl::RelativeMouseAxisUnit::get_state(AxisType type, [[maybe_unused]]int target, int axis, float)
{
    assert(target == 0);
    if(type == AxisType::general_axis)
    {
        return parent->relative_axes.get_raw(from_index<Axis2>(axis));
    }
    else if(type == AxisType::wheel)
    {
        return parent->wheels.get_raw(from_index<Axis2>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
        return 0.0f;
    }
}


void impl::AbsoluteMouseAxisUnit::register_axis(AxisType type, [[maybe_unused]] int target, int axis)
{
    assert(target == 0);
    if(type == AxisType::general_axis)
    {
        parent->absolute_axes.add(from_index<Axis2>(axis));
    }
    else if(type == AxisType::wheel)
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
float impl::AbsoluteMouseAxisUnit::get_state(AxisType type, [[maybe_unused]]int target, int axis, float)
{
    assert(target == 0);
    if(type == AxisType::general_axis)
    {
        return parent->absolute_axes.get_raw(from_index<Axis2>(axis));
    }
    else if(type == AxisType::wheel)
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


MouseActiveUnit::MouseActiveUnit(Director* d)
    : director(d)
{
    assert(director);

    director->add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


MouseActiveUnit::~MouseActiveUnit()
{
    director->remove(this);
}


KeyUnit* MouseActiveUnit::get_key_unit()
{
    return &key_unit;
}

AxisUnit* MouseActiveUnit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}

AxisUnit* MouseActiveUnit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}



bool MouseActiveUnit::is_considered_joystick()
{
    return false;
}


bool MouseActiveUnit::is_delete_scheduled()
{
    return false;
}


void MouseActiveUnit::on_axis(const Axis2& axis, float relative_state, float absolute_state)
{
    relative_axes.set_raw(axis, relative_state);
    absolute_axes.set_raw(axis, absolute_state);
}


void MouseActiveUnit::on_wheel(const Axis2& axis, float state)
{
    wheels.set_raw(axis, state);
}


void MouseActiveUnit::on_button(MouseButton button, float state)
{
    buttons.set_raw(button, state);
}


}  // namespace input
