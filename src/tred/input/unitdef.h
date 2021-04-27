#pragma once

#include <memory>
#include <optional>
#include <string>

#include "tred/input/unitdiscovery.h"
#include "tred/input/axistype.h"


namespace input
{


struct input_director;
struct active_unit;
struct Converter;
struct unit_setup;
struct platform;


struct unit_definition
{
    virtual ~unit_definition();

    virtual std::optional<std::string> validate_key(int key) = 0;
    virtual std::optional<std::string> validate_axis(axis_type type, int target, int axis) = 0;

    virtual bool is_considered_joystick() = 0;
    virtual bool can_detect(input_director* director, unit_discovery discovery, unit_setup* setup, platform* platform) = 0;
    virtual std::unique_ptr<active_unit> create(input_director* director, const unit_setup& setup) = 0;
};

}  // namespace input
