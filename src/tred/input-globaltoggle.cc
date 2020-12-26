#include "tred/input-globaltoggle.h"
#include <cassert>
#include <algorithm>

#include "tred/input-action.h"
#include "tred/input-bind.h"


namespace input
{
const float TRUE_VALUE = 0.8f;


GlobalToggle::GlobalToggle(std::shared_ptr<InputAction> action)
    : action_(action)
    , state_(false)
    , last_down_(false)
    , current_state_holder_(0)
{
    assert(action_);
    action_->Setup(this);
}


GlobalToggle::~GlobalToggle()
{
    assert(action_);
    action_->ClearToggle(this);
}


void GlobalToggle::Update()
{
    bool down;

    Bind* theBind = 0;

    if (current_state_holder_)
    {
        down = current_state_holder_->value() < TRUE_VALUE;
        theBind = current_state_holder_;
    }
    else
    {
        down = false;
        for (Bind* bind: binds_)
        {
            if (bind->value() > TRUE_VALUE)
            {
                down = true;
                theBind = bind;
            }
        }
    }

    // todo: toggle when released, instead of when pressed!
    if (down && !last_down_)
    {
        // toggle!
        if (state_)
        {
            state_ = false;
            current_state_holder_ = 0;
        }
        else
        {
            state_ = true;
            current_state_holder_ = theBind;
        }
    }
    last_down_ = down;
}


void GlobalToggle::Add(Bind* bind)
{
    binds_.push_back(bind);
}


void GlobalToggle::Remove(Bind* bind)
{
    binds_.erase(std::remove_if(binds_.begin(), binds_.end(), [bind](Bind* rhs) { return rhs == bind; }), binds_.end());
}


bool GlobalToggle::state() const
{
    return state_;
}


}  // namespace input
