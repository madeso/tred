#pragma once

#include "tred/input-active.h"


namespace input
{
struct InputAction;
struct Bind;

struct ActiveRangeToAxis : public InputActive
{
    ActiveRangeToAxis(InputAction* action, Bind* positive, Bind* negative);

    void Update(float dt);

    Bind* positive_;
    Bind* negative_;
};

}  // namespace input
