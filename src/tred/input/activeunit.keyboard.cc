#include "tred/input/activeunit.keyboard.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/bind.h"
#include "tred/input/index.h"


namespace input
{


void impl::KeyboardKeyUnit::register_key(int key)
{
    parent->keys.add(from_index<KeyboardKey>(key));
}


float impl::KeyboardKeyUnit::get_state(int key)
{
    return parent->keys.get_raw(from_index<KeyboardKey>(key));
}


KeyboardActiveUnit::KeyboardActiveUnit(Director* d)
    : director(d)
{
    assert(director);

    key_unit.parent = this;

    director->add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director->remove(this);
}


KeyUnit* KeyboardActiveUnit::get_key_unit()
{
    return &key_unit;
}


AxisUnit* KeyboardActiveUnit::get_relative_axis_unit()
{
    assert(false && "invalid call");
    return nullptr;
}


AxisUnit* KeyboardActiveUnit::get_absolute_axis_unit()
{
    assert(false && "invalid call");
    return nullptr;
}


bool KeyboardActiveUnit::is_considered_joystick()
{
    return false;
}


bool KeyboardActiveUnit::is_delete_scheduled()
{
    return false;
}


void KeyboardActiveUnit::on_key(const KeyboardKey& key, bool state)
{
    keys.set_raw(key, state ? 1.0f : 0.0f);
}


}  // namespace input
