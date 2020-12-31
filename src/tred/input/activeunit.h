#pragma once

#include <vector>
#include <memory>


namespace input
{

struct ValueReciever;

/** Represents a active unit.
A perfect example is a connected joystick.
 */
struct ActiveUnit
{
    virtual ~ActiveUnit();

    virtual void Recieve(ValueReciever* reciever) = 0;
};

}  // namespace input
