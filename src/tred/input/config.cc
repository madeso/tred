#include "tred/input/config.h"
#include "tred/input/index.h"


namespace input::config
{


action::action(const std::string& n, const std::string& v, input::range r)
    : name(n)
    , var(v)
    , range(r)
{
}


action::action(const std::string& nv, input::range r)
    : name(nv)
    , var(nv)
    , range(r)
{
}


action::action(const std::string_view& nv, input::range r)
    : name(nv)
    , var(nv)
    , range(r)
{
}


keyboard_definition::keyboard_definition()
    : detection_key(input::keyboard_key::unbound)
{
}


keyboard_definition::keyboard_definition(input::keyboard_key dk)
    : detection_key(dk)
{
}


mouse_definition::mouse_definition()
    : detection_button(input::mouse_button::unbound)
{
}


mouse_definition::mouse_definition(input::mouse_button db)
    : detection_button(db)
{
}


joystick_definition::joystick_definition()
    : start_button(0)
    , unit("")
{
}


joystick_definition::joystick_definition(int sb, const std::string& u)
    : start_button(sb)
    , unit(u)
{
}



unit_definition::unit_definition(const keyboard_definition& k)
    : keyboard(k)
{
}


unit_definition::unit_definition(const mouse_definition& m)
    : mouse(m)
{
}


unit_definition::unit_definition(const joystick_definition& j)
    : joystick(j)
{
}


unit_definition::unit_definition(const gamecontroller_definition& g)
    : gamecontroller(g)
{
}


key_bind_definition::key_bind_definition(const std::string& a, int u, input::keyboard_key k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

key_bind_definition::key_bind_definition(const std::string& a, int u, input::gamecontroller_button k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

key_bind_definition::key_bind_definition(const std::string& a, int u, input::mouse_button k)
    : action(a)
    , unit(u)
    , key(to_index(k))
{
}

key_bind_definition::key_bind_definition(const std::string& a, int u, int k)
    : action(a)
    , unit(u)
    , key(k)
{
}


axis_bind_definition::axis_bind_definition(const std::string& a, int u, input::xy_axis ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(axis_type::general_axis)
    , target(0)
    , axis(to_index(ax))
    , sensitivity(s)
    , is_inverted(ii)
{
}

axis_bind_definition::axis_bind_definition(const std::string& a, int u, input::gamecontroller_axis ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(axis_type::general_axis)
    , target(0)
    , axis(to_index(ax))
    , sensitivity(s)
    , is_inverted(ii)
{
}

axis_bind_definition::axis_bind_definition(const std::string& a, int u, axis_type ty, int ta, int ax, float s, bool ii)
    : action(a)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , sensitivity(s)
    , is_inverted(ii)
{
}


two_key_bind_definition::two_key_bind_definition(const std::string& a, int u, input::keyboard_key n, input::keyboard_key p)
    : action(a)
    , unit(u)
    , negative(to_index(n))
    , positive(to_index(p))
{
}

two_key_bind_definition::two_key_bind_definition(const std::string& a, int u, input::gamecontroller_button n, input::gamecontroller_button p)
    : action(a)
    , unit(u)
    , negative(to_index(n))
    , positive(to_index(p))
{
}

two_key_bind_definition::two_key_bind_definition(const std::string& a, int u, int n, int p)
    : action(a)
    , unit(u)
    , negative(n)
    , positive(p)
{
}


bind_definition::bind_definition(const key_bind_definition& k)
    : key(k)
{
}


bind_definition::bind_definition(const axis_bind_definition& a)
    : axis(a)
{
}


bind_definition::bind_definition(const two_key_bind_definition& t)
    : twokey(t)
{
}


mapping::mapping(const std::string& n, const std::vector<unit_definition>& u, const std::vector<bind_definition>& b)
    : name(n)
    , units(u)
    , binds(b)
{
}


input_system::input_system(const action_map& a, const mapping_list& k)
    : actions(a)
    , keys(k)
{
}


}
