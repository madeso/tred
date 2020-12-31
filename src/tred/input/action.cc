#include "tred/input/action.h"
#include <cassert>


namespace input
{


InputAction::InputAction(const std::string& n, const std::string& s, Range r)
    : name(n)
    , scriptvarname(s)
    , range(r)
{
}


}  // namespace input
