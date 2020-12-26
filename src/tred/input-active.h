#pragma once

struct Table;


namespace input
{
struct InputAction;

struct InputActive
{
    explicit InputActive(InputAction* action);

    float state() const;
    void set_state(float state);

    const InputAction& action() const;

    InputAction* action_;
    float state_;
};

}  // namespace input
