#include "tred/input-action.h"
#include <cassert>


namespace input
{
InputAction::InputAction(const std::string& n, const std::string& s, Range r, bool g)
    : name(n)
    , scriptvarname(s)
    , range(r)
    , global(g)
    , toggle(nullptr)
{
}

InputAction::~InputAction()
{
    assert(toggle == nullptr);
}

}  // namespace input
