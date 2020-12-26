#pragma once

#include <string>

#include "tred/input-range.h"


struct Table;

namespace input
{

struct GlobalToggle;

struct InputAction
{
    InputAction(const std::string& name, const std::string& scriptvarname, Range range, bool global);
    ~InputAction();

    std::string name;
    std::string scriptvarname;
    Range range;
    bool global;
    GlobalToggle* toggle;
};

}  // namespace input
