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
    std::string name;
    std::string var;
    ::Range range;
    bool global = false;
};

struct CommonDef
{
    std::string bindname;
    std::string actionname;
    // std::string type;
};

struct KeyboardButton
{
    CommonDef common;

    ::Key key;

    bool invert = false;
    float scale = 1.0f;
};

struct MouseAxis
{
    CommonDef common;

    ::Axis axis;

    bool invert = false;
    float scale = 1.0f;
};

struct MouseButton
{
    CommonDef common;

    ::MouseButton key;

    bool invert = false;
    float scale = 1.0f;
};


struct JoystickAxis
{
    CommonDef common;

    int axis;

    bool invert = false;
    float scale = 1.0f;
};

struct JoystickButton
{
    CommonDef common;

    int button;

    bool invert = false;
    float scale = 1.0f;
};

struct JoystickHat
{
    CommonDef common;

    int hat;
    Axis axis;

    bool invert = false;
    float scale = 1.0f;
};

///////////////////////////////////////////////////////////////////////////////

struct KeyboardDef
{
    std::vector<KeyboardButton> binds;
};

struct MouseDef
{
    std::vector<MouseAxis> axis;
    std::vector<MouseButton> button;
};
struct JoystickDef
{
    std::vector<JoystickAxis> axis;
    std::vector<JoystickButton> button;
    std::vector<JoystickHat> hat;
};

struct UnitInterface
{
    std::optional<KeyboardDef> keyboard;
    std::optional<MouseDef> mouse;
    std::optional<JoystickDef> joystick;
};

struct Config
{
    std::string name;
    std::vector<UnitInterface> units;
};

///////////////////////////////////////////////////////////////////////////////
// roots

struct ActionMap
{
    std::vector<Action> actions;
};

struct KeyConfigs
{
    std::vector<Config> configs;
};

struct InputSystem
{
    ActionMap actions;
    KeyConfigs keys;
    std::vector<std::string> players;
};

}
