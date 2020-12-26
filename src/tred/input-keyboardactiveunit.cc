#include "tred/input-keyboardactiveunit.h"

#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"
#include "tred/input-commondef.h"


namespace input
{

KeyboardActiveUnit::KeyboardActiveUnit(
        const std::vector<std::shared_ptr<TRangeBind<Key>>>& binds,
        InputDirector* d)
    : director(d)
    , actions(ConvertToBindMap<TRangeBind<Key>, Key>(binds))
{
    assert(director);

    director->Add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director->Remove(this);
}


void KeyboardActiveUnit::OnKey(const Key& key, bool state)
{
    auto found = actions.find(key);
    if (found != actions.end())
    {
        TransformAndSetBindValue(found->second, state ? 1.0f : 0.0f);
    }
}


void KeyboardActiveUnit::Rumble()
{
}


}  // namespace input
