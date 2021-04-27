#pragma once

#include <string>

#include "tred/input/range.h"


namespace input
{


struct input_action
{
    input_action(const std::string& name, const std::string& scriptvarname, range range);

    std::string name;
    std::string scriptvarname;
    input::range range;
};


}  // namespace input
