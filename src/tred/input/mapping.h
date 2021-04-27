#pragma once

#include <vector>
#include <memory>

#include "tred/input/bind.h"
#include "tred/input/unitdiscovery.h"


namespace input::config
{
    struct mapping;
}


namespace input
{


struct unit_definition;
struct input_director;
struct input_action_map;
struct connected_units;
struct unit_setup;
struct platform;


/** Contains a list of configurations.
    A good example is Mouse+Keyboard. The list could also be one gamepad.
    */
struct mapping
{
    mapping(const input_action_map& map, const config::mapping& root);
    ~mapping();

    void add(std::unique_ptr<unit_definition>&& unit);
    void add(std::unique_ptr<bind_definition>&& bind);

    bool is_any_considered_joystick();
    bool can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform);
    std::unique_ptr<connected_units> connect(input_director* director, const unit_setup& setup);

    std::vector<std::unique_ptr<unit_definition>> units;
    std::vector<std::unique_ptr<bind_definition>> binds;
};


}
