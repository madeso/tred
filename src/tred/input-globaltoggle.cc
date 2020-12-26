#include "tred/input-globaltoggle.h"
#include <cassert>
#include <algorithm>

#include "tred/input-action.h"
#include "tred/input-bind.h"


namespace input
{
const float TRUE_VALUE = 0.8f;


GlobalToggle::GlobalToggle(std::shared_ptr<InputAction> a)
    : action(a)
    , state(false)
    , last_down(false)
    , current_state_holder(0)
{
    assert(action);
    action->toggle = this;
}


GlobalToggle::~GlobalToggle()
{
    assert(action);
    action->toggle = nullptr;
}


void GlobalToggle::Update()
{
    bool down;

    Bind* theBind = 0;

    if (current_state_holder)
    {
        down = current_state_holder->value < TRUE_VALUE;
        theBind = current_state_holder;
    }
    else
    {
        down = false;
        for (Bind* bind: binds)
        {
            if (bind->value > TRUE_VALUE)
            {
                down = true;
                theBind = bind;
            }
        }
    }

    // todo: toggle when released, instead of when pressed!
    if (down && !last_down)
    {
        // toggle!
        if (state)
        {
            state = false;
            current_state_holder = 0;
        }
        else
        {
            state = true;
            current_state_holder = theBind;
        }
    }
    last_down = down;
}


void GlobalToggle::Add(Bind* bind)
{
    binds.push_back(bind);
}


void GlobalToggle::Remove(Bind* bind)
{
    binds.erase(std::remove_if(binds.begin(), binds.end(), [bind](Bind* rhs) { return rhs == bind; }), binds.end());
}



}  // namespace input
