#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/bind.h"
#include "tred/input/hataxis.h"


namespace input::config
{
struct gamecontroller_definition;
}


namespace input
{

struct input_director;
struct active_unit;
struct input_action_map;
struct platform;


struct gamecontroller_definition : public unit_definition
{
    gamecontroller_definition(int index, const config::gamecontroller_definition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(axis_type type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform) override;
    std::unique_ptr<active_unit> create(input_director* director, const unit_setup& setup) override;

    int index;
};


}  // namespace input
