#pragma once


namespace input
{

struct InputAction;
struct GlobalToggle;

enum class BindType
{
    RANGE,
    AXIS
};

struct Bind
{
    Bind(InputAction* action, BindType type);
    ~Bind();

    InputAction* action;
    BindType type;
    GlobalToggle* toggle;
    float value;
};

}  // namespace input
