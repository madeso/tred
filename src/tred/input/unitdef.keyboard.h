#pragma once

#include <vector>
#include <memory>

#include "tred/input/key.h"
#include "tred/input/unitdef.h"
#include "tred/input/bind.h"


namespace input::config
{
struct keyboard_definition;
}


namespace input
{


struct input_action_map;
struct active_unit;
struct input_director;

struct keyboard_definition : public unit_definition
{
    keyboard_definition(const config::keyboard_definition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(axis_type type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform) override;
    std::unique_ptr<active_unit> create(input_director* director, const unit_setup& setup) override;

    keyboard_key detection_key;
};


}  // namespace input
