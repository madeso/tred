#pragma once

#include <vector>
#include <memory>


namespace input
{

struct InputAction;

/** Represents a active unit.
A perfect example is a connected joystick.
 */
struct ActiveUnit
{
    virtual ~ActiveUnit();
};

}  // namespace input
