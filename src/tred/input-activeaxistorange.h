#pragma once

#include "tred/input-active.h"


namespace input
{

struct InputAction;
struct Bind;

struct ActiveAxisToRange : public InputActive
{
    ActiveAxisToRange(InputAction* action, Bind* axis);

    void Update(float dt);

    Bind* axis_;
};


}  // namespace input
