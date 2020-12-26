#include "tred/input-keyboardactiveunit.h"

#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"
#include "tred/input-commondef.h"


namespace input
{

KeyboardActiveUnit::KeyboardActiveUnit(
        const std::vector<std::shared_ptr<TRangeBind<Key>>>& binds,
        InputDirector* director)
    : director_(director)
    , actions_(ConvertToBindMap<TRangeBind<Key>, Key>(binds))
{
    assert(director_);

    director_->Add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director_->Remove(this);
}


void KeyboardActiveUnit::OnKey(const Key& key, bool state)
{
    auto actionsit = actions_.find(key);
    if (actionsit != actions_.end())
    {
        TransformAndSetBindValue(actionsit->second, state ? 1.0f : 0.0f);
    }
}


void KeyboardActiveUnit::Rumble()
{
}


}  // namespace input
