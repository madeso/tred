/** @file
Classes for input handling.
 */

#pragma once
#include <memory>
#include <cassert>

#include "tred/input-active.h"


namespace input
{
struct ActiveAxis;
struct ActiveRangeToAxis;

struct ActiveMasterAxis : public InputActive
{
    explicit ActiveMasterAxis(InputAction* action, ActiveAxis* axis, ActiveRangeToAxis* range);
    void Update(float dt);

    ActiveAxis* axis_;
    ActiveRangeToAxis* range_;
};

}  // namespace input
