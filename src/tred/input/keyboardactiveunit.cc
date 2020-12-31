#include "tred/input/keyboardactiveunit.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"
#include "tred/input/bind.h"


namespace input
{

KeyboardActiveUnit::KeyboardActiveUnit(InputDirector* d, const std::vector<BindDef<Key>>& k)
    : director(d)
    , keys(ConvertToBindMap(k))
{
    assert(director);

    director->Add(this);
}


KeyboardActiveUnit::~KeyboardActiveUnit()
{
    director->Remove(this);
}


void KeyboardActiveUnit::Recieve(ValueReciever* reciever)
{
    CallRecieve(keys, reciever);
}


void KeyboardActiveUnit::OnKey(const Key& key, bool state)
{
    auto found = keys.find(key);
    if (found != keys.end())
    {
        found->second.SetRawState(state ? 1.0f : 0.0f);
    }
}


}  // namespace input
