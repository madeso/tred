#include "tred/input/activeunit.gamecontroller.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::gamecontroller_key_unit::register_key(int key)
{
    parent->buttons.add(from_index<gamecontroller_button>(key));
}


float impl::gamecontroller_key_unit::get_state(int key)
{
    return parent->buttons.get_raw(from_index<gamecontroller_button>(key));
}


impl::gamecontroller_axis_unit::gamecontroller_axis_unit(bool ir)
    : is_relative(ir)
{
}


void impl::gamecontroller_axis_unit::register_axis(axis_type type, int target, int axis)
{
    switch(type)
    {
    case axis_type::general_axis:
        assert(target == 0);
        parent->axes.add(from_index<gamecontroller_axis>(axis));
        break;
    default:
        assert(false && "invalid type");
    }
}


float impl::gamecontroller_axis_unit::get_state(axis_type type, int target, int axis, float dt)
{
    switch(type)
    {
    case axis_type::general_axis:
        assert(target == 0);
        return smooth_axis(parent->axes.get_raw(from_index<gamecontroller_axis>(axis))) * dt;
    default:
        assert(false && "invalid type");
        return 0.0f;
    }
}


gamecontroller_active_unit::gamecontroller_active_unit(joystick_id j, input_director* d)
    : joystick(j)
    , director(d)
    , scheduled_delete(false)
    , relative_axis_unit(true)
    , absolute_axis_unit(false)
{
    assert(director);
    director->add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


gamecontroller_active_unit::~gamecontroller_active_unit()
{
    director->remove(this);
}


key_unit* gamecontroller_active_unit::get_key_unit()
{
    return &key_unit;
}


axis_unit* gamecontroller_active_unit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}


axis_unit* gamecontroller_active_unit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}


bool gamecontroller_active_unit::is_considered_joystick()
{
    return true;
}


bool gamecontroller_active_unit::is_delete_scheduled()
{
    return scheduled_delete;
}


void gamecontroller_active_unit::on_axis(gamecontroller_axis axis, float state)
{
    axes.set_raw(axis, state);
}


void gamecontroller_active_unit::on_button(gamecontroller_button button, float state)
{
    buttons.set_raw(button, smooth_range(state));
}


}  // namespace input
