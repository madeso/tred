#pragma once






#include "tred/input/range.h"
#include "tred/input/axis.h"
#include "tred/input/key.h"
#include "tred/input/axistype.h"


namespace input::config
{


struct Action
{
    Action(const std::string& name, const std::string& var, input::Range);
    Action(const std::string& name_and_var, input::Range);
    Action(const std::string_view& name_and_var, input::Range);

    std::string name;
    std::string var;
    input::Range range;
};


struct KeyboardDefinition
{
    KeyboardDefinition();
    KeyboardDefinition(KeyboardKey);

    KeyboardKey detection_key;
};


struct MouseDefinition
{
    MouseDefinition();
    MouseDefinition(input::MouseButton);

    input::MouseButton detection_button;
};


struct JoystickDefinition
{
    JoystickDefinition();
    JoystickDefinition(int, const std::string&);

    int start_button;
    std::string unit;
};


struct GamecontrollerDefinition
{
};


struct UnitDefinition
{
    UnitDefinition(const KeyboardDefinition&);
    UnitDefinition(const MouseDefinition&);
    UnitDefinition(const JoystickDefinition&);
    UnitDefinition(const GamecontrollerDefinition&);

    std::optional<KeyboardDefinition> keyboard;
    std::optional<MouseDefinition> mouse;
    std::optional<JoystickDefinition> joystick;
    std::optional<GamecontrollerDefinition> gamecontroller;
};


struct KeyBindDefinition
{
    KeyBindDefinition(const std::string&, int, input::KeyboardKey);
    KeyBindDefinition(const std::string&, int, input::GamecontrollerButton);
    KeyBindDefinition(const std::string&, int, input::MouseButton);
    KeyBindDefinition(const std::string&, int, int);

    std::string action;
    int unit;
    int key;
};


struct AxisBindDefinition
{
    AxisBindDefinition(const std::string&, int, input::Axis2, float sens = 1.0f, bool ii = false);
    AxisBindDefinition(const std::string&, int, input::GamecontrollerAxis, float sens = 1.0f, bool ii = false);
    AxisBindDefinition(const std::string&, int, input::AxisType, int, int, float sens=1.0f, bool ii = false);

    std::string action;
    int unit;
    AxisType type;
    int target; // (joystick axis, hat or ball index)
    int axis;
    float sensitivity;
    bool is_inverted;
};


struct TwoKeyBindDefinition
{
    TwoKeyBindDefinition(const std::string&, int, input::KeyboardKey, input::KeyboardKey);
    TwoKeyBindDefinition(const std::string&, int, input::GamecontrollerButton, input::GamecontrollerButton);
    TwoKeyBindDefinition(const std::string&, int, int, int);

    std::string action;
    int unit;
    int negative;
    int positive;
};


struct BindDefinition
{
    BindDefinition(const KeyBindDefinition&);
    BindDefinition(const AxisBindDefinition&);
    BindDefinition(const TwoKeyBindDefinition&);

    std::optional<KeyBindDefinition> key;
    std::optional<AxisBindDefinition> axis;
    std::optional<TwoKeyBindDefinition> twokey;
};


using unit_definitions = std::vector<UnitDefinition>;
using bind_definitions = std::vector<BindDefinition>;


struct Mapping
{
    Mapping(const std::string&, const std::vector<UnitDefinition>&, const std::vector<BindDefinition>&);

    std::string name;

    std::vector<UnitDefinition> units;
    std::vector<BindDefinition> binds;
};

///////////////////////////////////////////////////////////////////////////////
// roots

using ActionMap = std::vector<Action>;
using MappingList = std::vector<Mapping>;

struct InputSystem
{
    InputSystem(const ActionMap&, const MappingList&);

    ActionMap actions;
    MappingList keys;
};

}
