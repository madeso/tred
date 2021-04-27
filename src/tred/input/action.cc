#include "tred/input/action.h"
#include <cassert>


namespace input
{


input_action::input_action(const std::string& n, const std::string& s, input::range r)
    : name(n)
    , scriptvarname(s)
    , range(r)
{
}


}  // namespace input
