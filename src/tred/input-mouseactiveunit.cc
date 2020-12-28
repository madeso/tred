#include "tred/input-mouseactiveunit.h"

#include <cassert>

#include "tred/input-director.h"
#include "tred/input-action.h"


namespace input
{


MouseActiveUnit::MouseActiveUnit
    (
        InputDirector* d,
        const std::vector<BindDef<Axis>>& a,
        const std::vector<BindDef<MouseButton>>& b
    )
    : director(d)
    , axes(ConvertToBindMap(a))
    , buttons(ConvertToBindMap(b))
{
    assert(director);

    director->Add(this);
}


MouseActiveUnit::~MouseActiveUnit()
{
    director->Remove(this);
}


void MouseActiveUnit::Recieve(ValueReciever* reciever)
{
    CallRecieve(axes, reciever);
    CallRecieve(buttons, reciever);
}


void MouseActiveUnit::OnAxis(const Axis& key, float state)
{
    auto res = axes.find(key);
    if (res != axes.end())
    {
        res->second.SetRawState(state);
    }
}


void MouseActiveUnit::OnButton(MouseButton key, float state)
{
    auto res = buttons.find(key);
    if (res != buttons.end())
    {
        res->second.SetRawState(state);
    }
}


}  // namespace input
