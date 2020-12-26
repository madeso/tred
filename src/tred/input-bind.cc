#include "tred/input-bind.h"

#include <cassert>

#include "tred/input-action.h"
#include "tred/input-globaltoggle.h"


namespace input
{


Bind::Bind(InputAction* a, BindType t)
    : action(a)
    , type(t)
    , toggle(nullptr)
    , value(0.0f)
{
}


Bind::~Bind()
{
    if (toggle)
    {
        toggle->Remove(this);
    }
}

}  // namespace input
