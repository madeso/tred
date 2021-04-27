#pragma once

#include <vector>
#include <string>

#include "tred/types.h"


namespace input
{

enum class joystick_id : u64 {};

enum class joystick_type
{
    joystick, game_controller
};

struct platform
{
    virtual ~platform() = default;

    virtual std::vector<joystick_id> get_active_and_free_joysticks() = 0;

    virtual void start_using(joystick_id joy, joystick_type type) = 0;

    virtual bool match_unit(joystick_id joy, const std::string& unit) = 0;

    virtual void on_changed_connection(const std::string& config) = 0;
};

}
