#pragma once

struct Table;


namespace input
{
struct InputAction;

struct InputActive
{
    explicit InputActive(InputAction* action);

    float GetNormalizedState() const;

    InputAction* action;
    float state_;
};

}  // namespace input
