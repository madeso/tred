#include "tred/input-bind.h"

#include <cassert>

#include "tred/input-action.h"


namespace input
{


Bind::Bind(InputAction* a, BindType t)
    : action(a)
    , type(t)
    , value(0.0f)
{
}


}  // namespace input
