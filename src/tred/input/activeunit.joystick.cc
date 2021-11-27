#include "tred/input/activeunit.joystick.h"
#include "tred/assert.h"

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::JoystickKeyUnit::register_key(int key)
{
    parent->buttons.add(key);
}


float impl::JoystickKeyUnit::get_state(int key)
{
    return parent->buttons.get_raw(key);
}


impl::JoystickAxisUnit::JoystickAxisUnit(bool ir)
    : is_relative(ir)
{
}


void impl::JoystickAxisUnit::register_axis(AxisType type, int target, int axis)
{
    switch(type)
    {
    case AxisType::general_axis:
        ASSERT(target == 0);
        parent->axes.add(axis);
        break;
    case AxisType::hat:
        parent->hats.add({target, from_index<Axis2>(axis)});
        break;
    case AxisType::ball:
        parent->balls.add({target, from_index<Axis2>(axis)});
        break;
    default:
        DIE("invalid type");
    }
}


float impl::JoystickAxisUnit::get_state(AxisType type, int target, int axis, float dt)
{
    switch(type)
    {
    case AxisType::general_axis:
        ASSERT(target == 0);
        return smooth_axis(parent->axes.get_raw(axis)) * dt;
    case AxisType::hat:
        return parent->hats.get_raw({target, from_index<Axis2>(axis)}) * dt;
    case AxisType::ball:
        return parent->balls.get_raw({target, from_index<Axis2>(axis)}) * dt;
    default:
        DIE("invalid type");
        return 0.0f;
    }
}


JoystickActiveUnit::JoystickActiveUnit(JoystickId j, Director* d)
    : joystick(j)
    , director(d)
    , scheduled_delete(false)
    , relative_axis_unit(true)
    , absolute_axis_unit(false)
{
    ASSERT(director);
    director->add(this);

    key_unit.parent = this;
    relative_axis_unit.parent = this;
    absolute_axis_unit.parent = this;
}


JoystickActiveUnit::~JoystickActiveUnit()
{
    director->remove(this);
}


KeyUnit* JoystickActiveUnit::get_key_unit()
{
    return &key_unit;
}


AxisUnit* JoystickActiveUnit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}


AxisUnit* JoystickActiveUnit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}


bool JoystickActiveUnit::is_considered_joystick()
{
    return true;
}


bool JoystickActiveUnit::is_delete_scheduled()
{
    return scheduled_delete;
}


void JoystickActiveUnit::on_axis(int axis, float state)
{
    axes.set_raw(axis, state);
}


void JoystickActiveUnit::on_button(int button, float state)
{
    buttons.set_raw(button, state);
}


void JoystickActiveUnit::on_hat(const HatAndAxis2& hatAxis, float state)
{
    hats.set_raw(hatAxis, state);
}


void JoystickActiveUnit::on_ball(const HatAndAxis2& hatAxis, float state)
{
    balls.set_raw(hatAxis, state);
}


}  // namespace input
