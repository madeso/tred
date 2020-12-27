#include "tred/input-config.h"

namespace input::config
{

Action::Action(const std::string& n, const std::string& v, ::Range r)
    : name(n)
    , var(v)
    , range(r)
{
}


KeyboardButton::KeyboardButton(const std::string& bind, ::Key k, bool i, float s)
    : bindname(bind)
    , key(k)
    , invert(i)
    , scale(s)
{
}

MouseAxis::MouseAxis(const std::string& bind, ::Axis a, bool i, float s)
    : bindname(bind)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


MouseButton::MouseButton(const std::string& bind, ::MouseButton k, bool i, float s)
    : bindname(bind)
    , key(k)
    , invert(i)
    , scale(s)
{
}


JoystickAxis::JoystickAxis(const std::string& bind, int a, bool i, float s)
    : bindname(bind)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


JoystickButton::JoystickButton(const std::string& bind, int b, bool i, float s)
    : bindname(bind)
    , button(b)
    , invert(i)
    , scale(s)
{
}


JoystickHat::JoystickHat(const std::string& bind, int h, ::Axis a, bool i, float s)
    : bindname(bind)
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


Config::Config(const std::string& n, const std::vector<KeyboardDef>& k, const std::vector<MouseDef>& m, const std::vector<JoystickDef>& j)
    : name(n)
    , keyboards(k)
    , mouses(m)
    , joysticks(j)
{
}


InputSystem::InputSystem(const ActionMap& a, const KeyConfigs& k, const std::vector<std::string>& p)
    : actions(a)
    , keys(k)
    , players(p)
{
}


}
