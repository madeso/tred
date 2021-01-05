#pragma once

#include <vector>

#include "tred/types.h"


namespace input
{

enum class JoystickId : u64 {};

struct Platform
{
    virtual ~Platform() = default;

    virtual std::vector<JoystickId> ActiveJoysticks() = 0;
};

}
