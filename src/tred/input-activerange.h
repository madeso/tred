#pragma once

#include "tred/input-active.h"


namespace input
{
struct InputAction;
struct Bind;

struct ActiveRange : public InputActive
{
    ActiveRange(InputAction* action, Bind* range);

    void Update(float dt);

    Bind* range_;
};

}  // namespace input
