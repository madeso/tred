#include "tred/input/activeunit.joystick.h"
#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::joystick_key_unit::register_key(int key)
{
    parent->buttons.add(key);
}


float impl::joystick_key_unit::get_state(int key)
{
    return parent->buttons.get_raw(key);
}


impl::joystick_axis_unit::joystick_axis_unit(bool ir)
    : is_relative(ir)
{
}


void impl::joystick_axis_unit::register_axis(axis_type type, int target, int axis)
{
    switch(type)
    {
    case axis_type::general_axis:
        assert(target == 0);
        parent->axes.add(axis);
        break;
    case axis_type::hat:
        parent->hats.add({target, from_index<xy_axis>(axis)});
        break;
    case axis_type::ball:
        parent->balls.add({target, from_index<xy_axis>(axis)});
        break;
    default:
        assert(false && "invalid type");
    }
}


float impl::joystick_axis_unit::get_state(axis_type type, int target, int axis, float dt)
{
    switch(type)
    {
    case axis_type::general_axis:
        assert(target == 0);
        return smooth_axis(parent->axes.get_raw(axis)) * dt;
    case axis_type::hat:
        return parent->hats.get_raw({target, from_index<xy_axis>(axis)}) * dt;
    case axis_type::ball:
        return parent->balls.get_raw({target, from_index<xy_axis>(axis)}) * dt;
    default:
        assert(false && "invalid type");
        return 0.0f;
    }
}


joystick_active_unit::joystick_active_unit(joystick_id j, input_director* d)
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


joystick_active_unit::~joystick_active_unit()
{
    director->remove(this);
}


key_unit* joystick_active_unit::get_key_unit()
{
    return &key_unit;
}


axis_unit* joystick_active_unit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}


axis_unit* joystick_active_unit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}


bool joystick_active_unit::is_considered_joystick()
{
    return true;
}


bool joystick_active_unit::is_delete_scheduled()
{
    return scheduled_delete;
}


void joystick_active_unit::on_axis(int axis, float state)
{
    axes.set_raw(axis, state);
}


void joystick_active_unit::on_button(int button, float state)
{
    buttons.set_raw(button, state);
}


void joystick_active_unit::on_hat(const hat_and_xy_axis& hatAxis, float state)
{
    hats.set_raw(hatAxis, state);
}


void joystick_active_unit::on_ball(const hat_and_xy_axis& hatAxis, float state)
{
    balls.set_raw(hatAxis, state);
}


}  // namespace input
