#include "tred/input/activeunit.gamecontroller.h"
#include "tred/assert.h"

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/index.h"
#include "tred/input/smooth.h"


namespace input
{


void impl::GamecontrollerKeyUnit::register_key(int key)
{
    parent->buttons.add(from_index<GamecontrollerButton>(key));
}


float impl::GamecontrollerKeyUnit::get_state(int key)
{
    return parent->buttons.get_raw(from_index<GamecontrollerButton>(key));
}


impl::GamecontrollerAxisUnit::GamecontrollerAxisUnit(bool ir)
    : is_relative(ir)
{
}


void impl::GamecontrollerAxisUnit::register_axis(AxisType type, [[maybe_unused]] int target, int axis)
{
    switch(type)
    {
    case AxisType::general_axis:
        ASSERT(target == 0);
        parent->axes.add(from_index<GamecontrollerAxis>(axis));
        break;
    default:
        DIE("invalid type");
    }
}


float impl::GamecontrollerAxisUnit::get_state(AxisType type, [[maybe_unused]] int target, int axis, float dt)
{
    switch(type)
    {
    case AxisType::general_axis:
        ASSERT(target == 0);
        return smooth_axis(parent->axes.get_raw(from_index<GamecontrollerAxis>(axis))) * dt;
    default:
        DIE("invalid type");
        return 0.0f;
    }
}


GamecontrollerActiveUnit::GamecontrollerActiveUnit(JoystickId j, Director* d)
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


GamecontrollerActiveUnit::~GamecontrollerActiveUnit()
{
    director->remove(this);
}


KeyUnit* GamecontrollerActiveUnit::get_key_unit()
{
    return &key_unit;
}


AxisUnit* GamecontrollerActiveUnit::get_relative_axis_unit()
{
    return &relative_axis_unit;
}


AxisUnit* GamecontrollerActiveUnit::get_absolute_axis_unit()
{
    return &absolute_axis_unit;
}


bool GamecontrollerActiveUnit::is_considered_joystick()
{
    return true;
}


bool GamecontrollerActiveUnit::is_delete_scheduled()
{
    return scheduled_delete;
}


void GamecontrollerActiveUnit::on_axis(GamecontrollerAxis axis, float state)
{
    axes.set_raw(axis, state);
}


void GamecontrollerActiveUnit::on_button(GamecontrollerButton button, float state)
{
    buttons.set_raw(button, smooth_range(state));
}


}  // namespace input
