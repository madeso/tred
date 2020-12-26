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

    virtual void Rumble() = 0;
};

}  // namespace input
