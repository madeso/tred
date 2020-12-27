#pragma once


namespace input
{

struct InputAction;

enum class BindType
{
    RANGE,
    AXIS
};

struct Bind
{
    Bind(InputAction* action, BindType type);

    InputAction* action;
    BindType type;
    float value;
};

}  // namespace input
