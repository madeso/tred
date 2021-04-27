#include "tred/input/activeunit.mouse.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"


namespace input
{


void impl::mouse_key_unit::register_key(int key)
{
    parent->buttons.add(from_index<mouse_button>(key));
}


float impl::mouse_key_unit::get_state(int key)
{
    return parent->buttons.get_raw(from_index<mouse_button>(key));
}


void impl::relative_mouse_axis_unit::register_axis(axis_type type, int target, int axis)
{
    assert(target == 0);
    if(type == axis_type::general_axis)
    {
        parent->relative_axes.add(from_index<xy_axis>(axis));
    }
    else if(type == axis_type::wheel)
    {
        parent->wheels.add(from_index<xy_axis>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
    }
}


float impl::relative_mouse_axis_unit::get_state(axis_type type, int target, int axis, float)
{
    assert(target == 0);
    if(type == axis_type::general_axis)
    {
        return parent->relative_axes.get_raw(from_index<xy_axis>(axis));
    }
    else if(type == axis_type::wheel)
    {
        return parent->wheels.get_raw(from_index<xy_axis>(axis));
    }
    else
    {
        assert(false && "invalid mouse axis");
        return 0.0f;
    }
}


void impl::absolute_mouse_axis_unit::register_axis(axis_type type, int target, int axis)
{
    assert(target == 0);
    if(type == axis_type::general_axis)
    {
        parent->absolute_axes.add(from_index<xy_axis>(axis));
    }
    else if(type == axis_type::wheel)
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
float impl::absolute_mouse_axis_unit::get_state(axis_type type, int target, int axis, float)
{
    assert(target == 0);
    if(type == axis_type::general_axis)
    {
        return parent->absolute_axes.get_raw(from_index<xy_axis>(axis));
    }
    else if(type == axis_type::wheel)
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


mouse_active_unit::mouse_active_unit(input_director* d)
    : director(d)
{
    assert(director);

    director->add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


mouse_active_unit::~mouse_active_unit()
{
    director->remove(this);
}


key_unit* mouse_active_unit::get_key_unit()
{
    return &key_unit;
}

axis_unit* mouse_active_unit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}

axis_unit* mouse_active_unit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}



bool mouse_active_unit::is_considered_joystick()
{
    return false;
}


bool mouse_active_unit::is_delete_scheduled()
{
    return false;
}


void mouse_active_unit::on_axis(const xy_axis& axis, float relative_state, float absolute_state)
{
    relative_axes.set_raw(axis, relative_state);
    absolute_axes.set_raw(axis, absolute_state);
}


void mouse_active_unit::on_wheel(const xy_axis& axis, float state)
{
    wheels.set_raw(axis, state);
}


void mouse_active_unit::on_button(mouse_button button, float state)
{
    buttons.set_raw(button, state);
}


}  // namespace input
