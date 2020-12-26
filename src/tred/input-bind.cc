#include "tred/input-bind.h"

#include <cassert>

#include "tred/input-action.h"
#include "tred/input-globaltoggle.h"


namespace input
{


Bind::~Bind()
{
    if (toggle_)
    {
        toggle_->Remove(this);
    }
}


float Bind::value() const
{
    return value_;
}


void Bind::set_value(float v)
{
    value_ = v;
}


BindType Bind::type() const
{
    return type_;
}


InputAction* Bind::action()
{
    assert(action_);
    return action_;
}


}  // namespace input
