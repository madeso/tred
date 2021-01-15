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

    virtual KeyUnit* GetKeyUnit() = 0;
    virtual AxisUnit* GetRelativeAxisUnit() = 0;
    virtual AxisUnit* GetAbsoluteAxisUnit() = 0;

    virtual bool IsConsideredJoystick() = 0;
    virtual bool IsDeleteSheduled() = 0;
};

}  // namespace input
