#pragma once

#include <vector>
#include <memory>


namespace input
{

struct KeyUnit;
struct AxisUnit;


/** Represents a active unit.
A perfect example is a connected joystick.
 */
struct ActiveUnit
{
    virtual ~ActiveUnit();

    virtual KeyUnit* get_key_unit() = 0;
    virtual AxisUnit* get_relative_axis_unit() = 0;
    virtual AxisUnit* get_absolute_axis_unit() = 0;

    virtual bool is_considered_joystick() = 0;
    virtual bool is_delete_scheduled() = 0;
};

}  // namespace input
