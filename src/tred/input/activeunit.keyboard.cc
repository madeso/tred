#include "tred/input/activeunit.keyboard.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/bind.h"
#include "tred/input/index.h"


namespace input
{


void impl::KeyboardKeyUnit::RegisterKey(int key)
{
    parent->keys.Add(FromIndex<Key>(key));
}


float impl::KeyboardKeyUnit::GetState(int key)
{
    return parent->keys.GetRaw(FromIndex<Key>(key));
}


KeyboardActiveUnit::KeyboardActiveUnit(InputDirector* d)
    : director(d)
{
    assert(director);

    key_unit.parent = this;

    director->Add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director->Remove(this);
}


KeyUnit* KeyboardActiveUnit::GetKeyUnit()
{
    return &key_unit;
}


AxisUnit* KeyboardActiveUnit::GetRelativeAxisUnit()
{
    assert(false && "invalid call");
    return nullptr;
}


AxisUnit* KeyboardActiveUnit::GetAbsoluteAxisUnit()
{
    assert(false && "invalid call");
    return nullptr;
}


bool KeyboardActiveUnit::IsConsideredJoystick()
{
    return false;
}


bool KeyboardActiveUnit::IsDeleteSheduled()
{
    return false;
}


void KeyboardActiveUnit::OnKey(const Key& key, bool state)
{
    keys.SetRaw(key, state ? 1.0f : 0.0f);
}


}  // namespace input
