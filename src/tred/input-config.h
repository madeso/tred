#pragma once

#include <vector>
#include <string>
#include <optional>

#include "tred/input-range.h"
#include "tred/input-axis.h"
#include "tred/input-key.h"

namespace input::config
{
struct Action
{
    Action(const std::string& name, const std::string& var, ::Range, bool global=false);
    std::string name;
    std::string var;
    ::Range range;
    bool global;
};

struct CommonDef
{
    CommonDef(const std::string& bind, const std::string& action);

    std::string bindname;
    std::string actionname;
    // std::string type;
};

struct KeyboardButton
{
    KeyboardButton(const CommonDef&, ::Key, bool invert=false, float scale=1.0f);

    CommonDef common;

    ::Key key;

    bool invert;
    float scale;
};

struct MouseAxis
{
    MouseAxis(const CommonDef&, ::Axis, bool invert=false, float scale=1.0f);

    CommonDef common;

    ::Axis axis;

    bool invert;
    float scale;
};

struct MouseButton
{
    MouseButton(const CommonDef&, ::MouseButton, bool invert=false, float scale=1.0f);
    CommonDef common;

    ::MouseButton key;

    bool invert;
    float scale;
};


struct JoystickAxis
{
    JoystickAxis(const CommonDef&, int, bool invert=false, float scale=1.0f);

    CommonDef common;

    int axis;

    bool invert;
    float scale;
};

struct JoystickButton
{
    JoystickButton(const CommonDef&, int, bool invert=false, float scale=1.0f);
    CommonDef common;

    int button;

    bool invert;
    float scale;
};

struct JoystickHat
{
    JoystickHat(const CommonDef&, int, ::Axis, bool invert=false, float scale=1.0f);

    CommonDef common;

    int hat;
    ::Axis axis;

    bool invert;
    float scale;
};

///////////////////////////////////////////////////////////////////////////////

struct KeyboardDef
{
    KeyboardDef();
    KeyboardDef(const std::vector<KeyboardButton>&);

    std::vector<KeyboardButton> binds;
};

struct MouseDef
{
    MouseDef();
    MouseDef(const std::vector<MouseAxis>&, const std::vector<MouseButton>&);

    std::vector<MouseAxis> axis;
    std::vector<MouseButton> button;
};

struct JoystickDef
{
    JoystickDef();
    JoystickDef(const std::vector<JoystickAxis>&, const std::vector<JoystickButton>&, const std::vector<JoystickHat>&);

    std::vector<JoystickAxis> axis;
    std::vector<JoystickButton> button;
    std::vector<JoystickHat> hat;
};


// todo(Gustav): move one layer up
struct UnitInterface
{
    UnitInterface(const KeyboardDef&);
    UnitInterface(const MouseDef&);
    UnitInterface(const JoystickDef&);

    std::optional<KeyboardDef> keyboard;
    std::optional<MouseDef> mouse;
    std::optional<JoystickDef> joystick;
};

struct Config
{
    Config(const std::string&, const std::vector<UnitInterface>&);

    std::string name;
    std::vector<UnitInterface> units;
};

///////////////////////////////////////////////////////////////////////////////
// roots

using ActionMap = std::vector<Action>;
using KeyConfigs = std::vector<Config>;

struct InputSystem
{
    InputSystem(const ActionMap&, const KeyConfigs&, const std::vector<std::string>&);

    ActionMap actions;
    KeyConfigs keys;
    std::vector<std::string> players;
};

}
