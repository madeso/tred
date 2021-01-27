#include "tred/input/config.h"
#include "tred/input/index.h"


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


KeyboardDef::KeyboardDef()
    : detection_key(input::Key::UNBOUND)
{
}


KeyboardDef::KeyboardDef(input::Key dk)
    : detection_key(dk)
{
}


MouseDef::MouseDef()
    : detection_button(input::MouseButton::UNBOUND)
{
}


MouseDef::MouseDef(input::MouseButton db)
    : detection_button(db)
{
}


JoystickDef::JoystickDef()
    : start_button(0)
    , unit("")
{
}


JoystickDef::JoystickDef(int sb, const std::string& u)
    : start_button(sb)
    , unit(u)
{
}



UnitDef::UnitDef(const KeyboardDef& k)
    : keyboard(k)
{
}


UnitDef::UnitDef(const MouseDef& m)
    : mouse(m)
{
}


UnitDef::UnitDef(const JoystickDef& j)
    : joystick(j)
{
}


KeyBindDef::KeyBindDef(const std::string& a, int u, input::Key k)
    : action(a)
    , unit(u)
    , key(ToIndex(k))
{
}

KeyBindDef::KeyBindDef(const std::string& a, int u, input::MouseButton k)
    : action(a)
    , unit(u)
    , key(ToIndex(k))
{
}

KeyBindDef::KeyBindDef(const std::string& a, int u, int k)
    : action(a)
    , unit(u)
    , key(k)
{
}


AxisBindDef::AxisBindDef(const std::string& a, int u, input::Axis ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(AxisType::GeneralAxis)
    , target(0)
    , axis(ToIndex(ax))
    , sensitivity(s)
    , is_inverted(ii)
{
}

AxisBindDef::AxisBindDef(const std::string& a, int u, AxisType ty, int ta, int ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , sensitivity(s)
    , is_inverted(ii)
{
}


TwoKeyBindDef::TwoKeyBindDef(const std::string& a, int u, input::Key n, input::Key p)
    : action(a)
    , unit(u)
    , negative(ToIndex(n))
    , positive(ToIndex(p))
{
}

TwoKeyBindDef::TwoKeyBindDef(const std::string& a, int u, int n, int p)
    : action(a)
    , unit(u)
    , negative(n)
    , positive(p)
{
}


BindDef::BindDef(const KeyBindDef& k)
    : key(k)
{
}


BindDef::BindDef(const AxisBindDef& a)
    : axis(a)
{
}


BindDef::BindDef(const TwoKeyBindDef& t)
    : twokey(t)
{
}


Mapping::Mapping(const std::string& n, const std::vector<UnitDef>& u, const std::vector<BindDef>& b)
    : name(n)
    , units(u)
    , binds(b)
{
}


InputSystem::InputSystem(const ActionMap& a, const MappingList& k)
    : actions(a)
    , keys(k)
{
}


}
