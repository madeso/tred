#include "tred/input/keyboardactiveunit.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/bind.h"


namespace input
{

KeyboardActiveUnit::KeyboardActiveUnit(InputDirector* d, Converter* converter, const std::vector<BindDef<Key>>& k)
    : director(d)
    , keys(k, converter)
{
    assert(director);

    director->Add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director->Remove(this);
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
