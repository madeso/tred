#pragma once

#include <vector>
#include <memory>


namespace input
{

struct key_unit;
struct axis_unit;


/** Represents a active unit.
A perfect example is a connected joystick.
 */
struct active_unit
{
    virtual ~active_unit();

    virtual key_unit* get_key_unit() = 0;
    virtual axis_unit* get_relative_axis_unit() = 0;
    virtual axis_unit* get_absolute_axis_unit() = 0;

    virtual bool is_considered_joystick() = 0;
    virtual bool is_delete_scheduled() = 0;
};

}  // namespace input
