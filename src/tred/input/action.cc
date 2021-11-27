#include "tred/input/action.h"
#include "tred/assert.h"


namespace input
{


Action::Action(const std::string& n, const std::string& s, input::Range r)
    : name(n)
    , scriptvarname(s)
    , range(r)
{
}


}  // namespace input
