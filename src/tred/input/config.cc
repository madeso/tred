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


KeyboardDefinition::KeyboardDefinition()
    : detection_key(input::KeyboardKey::unbound)
{
}


KeyboardDefinition::KeyboardDefinition(input::KeyboardKey dk)
    : detection_key(dk)
{
}


MouseDefinition::MouseDefinition()
    : detection_button(input::MouseButton::unbound)
{
}


MouseDefinition::MouseDefinition(input::MouseButton db)
    : detection_button(db)
{
}


JoystickDefinition::JoystickDefinition()
    : start_button(0)
    , unit("")
{
}


JoystickDefinition::JoystickDefinition(int sb, const std::string& u)
    : start_button(sb)
    , unit(u)
{
}



UnitDefinition::UnitDefinition(const KeyboardDefinition& k)
    : keyboard(k)
{
}


UnitDefinition::UnitDefinition(const MouseDefinition& m)
    : mouse(m)
{
}


UnitDefinition::UnitDefinition(const JoystickDefinition& j)
    : joystick(j)
{
}


UnitDefinition::UnitDefinition(const GamecontrollerDefinition& g)
    : gamecontroller(g)
{
}


KeyBindDefinition::KeyBindDefinition(const std::string& a, int u, input::KeyboardKey k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

KeyBindDefinition::KeyBindDefinition(const std::string& a, int u, input::GamecontrollerButton k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

KeyBindDefinition::KeyBindDefinition(const std::string& a, int u, input::MouseButton k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

KeyBindDefinition::KeyBindDefinition(const std::string& a, int u, int k)
    : action(a)
    , unit(u)
    , key(k)
{
}


AxisBindDefinition::AxisBindDefinition(const std::string& a, int u, input::Axis2 ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(AxisType::general_axis)
    , target(0)
    , axis(to_index(ax))
    , sensitivity(s)
    , is_inverted(ii)
{
}

AxisBindDefinition::AxisBindDefinition(const std::string& a, int u, input::GamecontrollerAxis ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(AxisType::general_axis)
    , target(0)
    , axis(to_index(ax))
    , sensitivity(s)
    , is_inverted(ii)
{
}

AxisBindDefinition::AxisBindDefinition(const std::string& a, int u, AxisType ty, int ta, int ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , sensitivity(s)
    , is_inverted(ii)
{
}


TwoKeyBindDefinition::TwoKeyBindDefinition(const std::string& a, int u, input::KeyboardKey n, input::KeyboardKey p)
    : action(a)
    , unit(u)
    , negative(to_index(n))
    , positive(to_index(p))
{
}

TwoKeyBindDefinition::TwoKeyBindDefinition(const std::string& a, int u, input::GamecontrollerButton n, input::GamecontrollerButton p)
    : action(a)
    , unit(u)
    , negative(to_index(n))
    , positive(to_index(p))
{
}

TwoKeyBindDefinition::TwoKeyBindDefinition(const std::string& a, int u, int n, int p)
    : action(a)
    , unit(u)
    , negative(n)
    , positive(p)
{
}


BindDefinition::BindDefinition(const KeyBindDefinition& k)
    : key(k)
{
}


BindDefinition::BindDefinition(const AxisBindDefinition& a)
    : axis(a)
{
}


BindDefinition::BindDefinition(const TwoKeyBindDefinition& t)
    : twokey(t)
{
}


Mapping::Mapping(const std::string& n, const std::vector<UnitDefinition>& u, const std::vector<BindDefinition>& b)
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
