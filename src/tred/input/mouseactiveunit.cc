#include "tred/input/mouseactiveunit.h"

#include <cassert>

#include "tred/input/director.h"
#include "tred/input/action.h"


namespace input
{


MouseActiveUnit::MouseActiveUnit
    (
        InputDirector* d, Converter* converter,
        const std::vector<BindDef<Axis>>& a,
        const std::vector<BindDef<MouseButton>>& b
    )
    : director(d)
    , axes(a, converter)
    , buttons(b, converter)
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
    axes.Recieve(reciever);
    buttons.Recieve(reciever);
}


void MouseActiveUnit::OnAxis(const Axis& axis, float state)
{
    axes.SetRaw(axis, state);
}


void MouseActiveUnit::OnButton(MouseButton button, float state)
{
    buttons.SetRaw(button, state);
}


}  // namespace input
