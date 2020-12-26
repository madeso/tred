#include "tred/input-bind.h"
#include <cassert>
#include "tred/input-action.h"
#include "tred/input-globaltoggle.h"


namespace input
{
std::string ToString(const BindType& bt)
{
    switch (bt)
    {
    case BindType::AXIS:
        return "axis";
    case BindType::RANGE:
        return "range";
    default:
        return "<unknown BindType>";
    }
}

std::ostream& operator<<(std::ostream& ss, const BindType& bt)
{
    ss << ToString(bt);
    return ss;
}

Bind::Bind(InputAction* action, BindType type)
    : action_(action)
    , type_(type)
    , toggle_(action ? action->toggle() : 0)
    , value_(0.0f)
{
    assert(action_);

    // if the action is a global, we should have a toggle
    assert(action_->global() == (toggle_ != 0));

    if (toggle_)
    {
        assert(action->global());  // the action should be global
        toggle_->Add(this);
    }
}

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
