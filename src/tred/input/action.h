#pragma once



#include "tred/input/range.h"


namespace input
{


struct Action
{
    Action(const std::string& name, const std::string& scriptvarname, Range range);

    std::string name;
    std::string scriptvarname;
    input::Range range;
};


}  // namespace input
