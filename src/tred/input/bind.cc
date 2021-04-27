#include "tred/input/bind.h"

#include <cassert>

#include "tred/input/table.h"
#include "tred/input/activeunit.h"
#include "tred/input/connectedunits.h"
#include "tred/log.h"


namespace input
{


float keep_within(float mi, float v, float ma)
{
    if (v < mi) { return mi; }
    else if (v > ma) { return ma; }
    else { return v; }
}


struct key_active_bind : public active_bind
{
    std::string var;
    int button;
    key_unit* unit;

    key_active_bind(const std::string& v, int b, key_unit* u)
        : var(v)
        , button(b)
        , unit(u)
    {
        unit->register_key(button);
    }

    void set_value_in_table(table* t, float) override
    {
        const auto state = unit->get_state(button);
        t->set(var, keep_within(0.0f, state, 1.0f));
    }
};



struct axis_active_bind : public active_bind
{
    std::string var;
    axis_type type;
    int target;
    int axis;
    axis_unit* unit;
    bool keep_within;
    bool is_inverted;
    float sensitivity;

    axis_active_bind(const std::string& v, axis_type ty, int ta, int ax, axis_unit* u, bool kw, bool i, float s)
        : var(v)
        , type(ty)
        , target(ta)
        , axis(ax)
        , unit(u)
        , keep_within(kw)
        , is_inverted(i)
        , sensitivity(s)
    {
        unit->register_axis(type, target, axis);
    }

    void set_value_in_table(table* t, float dt) override
    {
        const auto invert_scale = is_inverted ? -1.0f : 1.0f;
        const auto state = unit->get_state(type, target, axis, keep_within ? 1.0f : dt) * invert_scale;
        t->set(var, keep_within ? input::keep_within(-1.0f, state, 1.0f) : state * sensitivity);
    }
};



struct two_key_active_bind : public active_bind
{
    std::string var;
    int positive;
    int negative;
    key_unit* unit;
    bool keep_within;

    two_key_active_bind(const std::string& v, int p, int n, key_unit* u, bool kw)
        : var(v)
        , positive(p)
        , negative(n)
        , unit(u)
        , keep_within(kw)
    {
        unit->register_key(positive);
        unit->register_key(negative);
    }

    void set_value_in_table(table* t, float dt) override
    {
        const auto state = unit->get_state(positive) - unit->get_state(negative);
        t->set(var, keep_within ? input::keep_within(-1.0f, state, 1.0f) : state * dt);
    }
};



key_bind_definition::key_bind_definition(const std::string& v, int u, int k)
    : var(v)
    , unit(u)
    , key(k)
{
}


std::unique_ptr<active_bind> key_bind_definition::create(connected_units* units)
{
    auto* active_unit = units->get_unit(unit);
    assert(active_unit);
    return std::make_unique<key_active_bind>(var, key, active_unit->get_key_unit());
}


relative_axis_bind_definition::relative_axis_bind_definition(const std::string& v, int u, axis_type ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<active_bind> relative_axis_bind_definition::create(connected_units* units)
{
    auto* active_unit = units->get_unit(unit);
    assert(active_unit);

    return std::make_unique<axis_active_bind>(var, type, target, axis, active_unit->get_relative_axis_unit(), false, is_inverted, sensitivity);
}


absolute_axis_bind_definition::absolute_axis_bind_definition(const std::string& v, int u, axis_type ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<active_bind> absolute_axis_bind_definition::create(connected_units* units)
{
    auto* active_unit = units->get_unit(unit);
    assert(active_unit);
    return std::make_unique<axis_active_bind>(var, type, target, axis, active_unit->get_absolute_axis_unit(), true, is_inverted, sensitivity);
}


relative_two_key_bind_definition::relative_two_key_bind_definition(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<active_bind> relative_two_key_bind_definition::create(connected_units* units)
{
    auto* active_unit = units->get_unit(unit);
    assert(active_unit);
    return std::make_unique<two_key_active_bind>(var, positive, negative, active_unit->get_key_unit(), false);
}


absolute_two_key_bind_definition::absolute_two_key_bind_definition(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<active_bind> absolute_two_key_bind_definition::create(connected_units* units)
{
    auto* active_unit = units->get_unit(unit);
    assert(active_unit);
    return std::make_unique<two_key_active_bind>(var, positive, negative, active_unit->get_key_unit(), true);
}


}
