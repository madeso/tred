#pragma once

#include <string>

#include "tred/input-range.h"


struct Table;

namespace input
{


struct InputAction
{
    InputAction(const std::string& name, const std::string& scriptvarname, Range range);

    std::string name;
    std::string scriptvarname;
    Range range;
};


}  // namespace input
