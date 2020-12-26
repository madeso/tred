#include "tred/input-config.h"

namespace input::config
{

Action::Action(const std::string& n, const std::string& v, ::Range r, bool g)
    : name(n)
    , var(v)
    , range(r)
    , global(g)
{
}


CommonDef::CommonDef(const std::string& b, const std::string& a)
    : bindname(b)
    , actionname(a)
{
}


KeyboardButton::KeyboardButton(const CommonDef& c, ::Key k, bool i, float s)
    : common(c)
    , key(k)
    , invert(i)
    , scale(s)
{
}

MouseAxis::MouseAxis(const CommonDef& c, ::Axis a, bool i, float s)
    : common(c)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


MouseButton::MouseButton(const CommonDef& c, ::MouseButton k, bool i, float s)
    : common(c)
    , key(k)
    , invert(i)
    , scale(s)
{
}


JoystickAxis::JoystickAxis(const CommonDef& c, int a, bool i, float s)
    : common(c)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


JoystickButton::JoystickButton(const CommonDef& c, int b, bool i, float s)
    : common(c)
    , button(b)
    , invert(i)
    , scale(s)
{
}


JoystickHat::JoystickHat(const CommonDef& c, int h, ::Axis a, bool i, float s)
    : common(c)
    , hat(h)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


KeyboardDef::KeyboardDef()
{
}


KeyboardDef::KeyboardDef(const std::vector<KeyboardButton>& b)
    : binds(b)
{
}


MouseDef::MouseDef()
{
}


MouseDef::MouseDef(const std::vector<MouseAxis>& a, const std::vector<MouseButton>& b)
    : axis(a)
    , button(b)
{
}


JoystickDef::JoystickDef()
{
}


JoystickDef::JoystickDef(const std::vector<JoystickAxis>& a, const std::vector<JoystickButton>& b, const std::vector<JoystickHat>& h)
    : axis(a)
    , button(b)
    , hat(h)
{
}


UnitInterface::UnitInterface(const KeyboardDef& k)
    : keyboard(k)
{
}


UnitInterface::UnitInterface(const MouseDef& m)
    : mouse(m)
{
}


UnitInterface::UnitInterface(const JoystickDef& j)
    : joystick(j)
{
}


Config::Config(const std::string& n, const std::vector<UnitInterface>& u)
    : name(n)
    , units(u)
{
}


InputSystem::InputSystem(const ActionMap& a, const KeyConfigs& k, const std::vector<std::string>& p)
    : actions(a)
    , keys(k)
    , players(p)
{
}


}
