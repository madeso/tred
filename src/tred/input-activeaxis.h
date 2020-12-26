#pragma once

#include "tred/input-active.h"


namespace input
{
struct InputAction;
struct Bind;

struct ActiveAxis : public InputActive
{
    ActiveAxis(InputAction* action, Bind* axis);

    void Update(float dt);

    Bind* axis_;
};


}  // namespace input
