#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/axis.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"


namespace input::config
{
struct mouse_definition;
}


namespace input
{


struct input_action_map;


struct mouse_definition : public unit_definition
{
    mouse_definition(const config::mouse_definition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(axis_type type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform) override;
    std::unique_ptr<active_unit> create(input_director* director, const unit_setup& setup) override;

    mouse_button detection_button;
};


}
