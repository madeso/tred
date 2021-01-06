#include "tred/input/config.h"

namespace input::config
{

Action::Action(const std::string& n, const std::string& v, input::Range r)
    : name(n)
    , var(v)
    , range(r)
{
}


Action::Action(const std::string& nv, input::Range r)
    : name(nv)
    , var(nv)
    , range(r)
{
}


Action::Action(const std::string_view& nv, input::Range r)
    : name(nv)
    , var(nv)
    , range(r)
{
}


KeyboardButton::KeyboardButton(const std::string& bind, input::Key k, bool i, float s)
    : bindname(bind)
    , key(k)
    , invert(i)
    , scale(s)
{
}

MouseAxis::MouseAxis(const std::string& bind, input::Axis a, bool i, float s)
    : bindname(bind)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


MouseButton::MouseButton(const std::string& bind, input::MouseButton k, bool i, float s)
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


JoystickHat::JoystickHat(const std::string& bind, int h, input::Axis a, bool i, float s)
    : bindname(bind)
    , hat(h)
    , axis(a)
    , invert(i)
    , scale(s)
{
}


KeyboardDef::KeyboardDef()
    : detection_key(input::Key::UNBOUND)
{
}


KeyboardDef::KeyboardDef(const std::vector<KeyboardButton>& b)
    : detection_key(input::Key::UNBOUND)
    , binds(b)
{
}


KeyboardDef::KeyboardDef(input::Key dk, const std::vector<KeyboardButton>& b)
    : detection_key(dk)
    , binds(b)
{
}


MouseDef::MouseDef()
    : detection_button(input::MouseButton::UNBOUND)
{
}


MouseDef::MouseDef(const std::vector<MouseAxis>& a, const std::vector<MouseAxis>& w, const std::vector<MouseButton>& b)
    : detection_button(input::MouseButton::UNBOUND)
    , axis(a)
    , wheel(w)
    , button(b)
{
}


MouseDef::MouseDef(input::MouseButton db, const std::vector<MouseAxis>& a, const std::vector<MouseAxis>& w, const std::vector<MouseButton>& b)
    : detection_button(db)
    , axis(a)
    , wheel(w)
    , button(b)
{
}


JoystickDef::JoystickDef()
    : start_button(0)
    , unit("")
{
}


JoystickDef::JoystickDef(int sb, const std::string& u, const std::vector<JoystickAxis>& a, const std::vector<JoystickButton>& b, const std::vector<JoystickHat>& h, const std::vector<JoystickHat>& ba)
    : start_button(sb)
    , unit(u)
    , axis(a)
    , button(b)
    , hat(h)
    , ball(ba)
{
}


Mapping::Mapping(const std::string& n, const std::vector<TwoButtonConverter>& tbc, const std::vector<KeyboardDef>& k, const std::vector<MouseDef>& m, const std::vector<JoystickDef>& j)
    : name(n)
    , two_button_converter(tbc)
    , keyboards(k)
    , mouses(m)
    , joysticks(j)
{
}


InputSystem::InputSystem(const ActionMap& a, const MappingList& k)
    : actions(a)
    , keys(k)
{
}


}
