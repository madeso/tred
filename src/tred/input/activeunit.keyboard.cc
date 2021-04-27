#include "tred/input/activeunit.keyboard.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/bind.h"
#include "tred/input/index.h"


namespace input
{


void impl::keyboard_key_unit::register_key(int key)
{
    parent->keys.add(from_index<keyboard_key>(key));
}


float impl::keyboard_key_unit::get_state(int key)
{
    return parent->keys.get_raw(from_index<keyboard_key>(key));
}


keyboard_active_unit::keyboard_active_unit(input_director* d)
    : director(d)
{
    assert(director);

    key_unit.parent = this;

    director->add(this);
}


keyboard_active_unit::~keyboard_active_unit()
{
    director->remove(this);
}


key_unit* keyboard_active_unit::get_key_unit()
{
    return &key_unit;
}


axis_unit* keyboard_active_unit::get_relative_axis_unit()
{
    assert(false && "invalid call");
    return nullptr;
}


axis_unit* keyboard_active_unit::get_absolute_axis_unit()
{
    assert(false && "invalid call");
    return nullptr;
}


bool keyboard_active_unit::is_considered_joystick()
{
    return false;
}


bool keyboard_active_unit::is_delete_scheduled()
{
    return false;
}


void keyboard_active_unit::on_key(const keyboard_key& key, bool state)
{
    keys.set_raw(key, state ? 1.0f : 0.0f);
}


}  // namespace input
