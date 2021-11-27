#include "tred/input/bind.h"

#include "tred/assert.h"

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


struct KeyActiveBind : public ActiveBind
{
    std::string var;
    int button;
    KeyUnit* unit;

    KeyActiveBind(const std::string& v, int b, KeyUnit* u)
        : var(v)
        , button(b)
        , unit(u)
    {
        unit->register_key(button);
    }

    void set_value_in_table(Table* t, float) override
    {
        const auto state = unit->get_state(button);
        t->set(var, keep_within(0.0f, state, 1.0f));
    }
};



struct AxisActiveBind : public ActiveBind
{
    std::string var;
    AxisType type;
    int target;
    int axis;
    AxisUnit* unit;
    bool keep_within;
    bool is_inverted;
    float sensitivity;

    AxisActiveBind(const std::string& v, AxisType ty, int ta, int ax, AxisUnit* u, bool kw, bool i, float s)
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

    void set_value_in_table(Table* t, float dt) override
    {
        const auto invert_scale = is_inverted ? -1.0f : 1.0f;
        const auto state = unit->get_state(type, target, axis, keep_within ? 1.0f : dt) * invert_scale;
        t->set(var, keep_within ? input::keep_within(-1.0f, state, 1.0f) : state * sensitivity);
    }
};



struct TwoKeyActiveBind : public ActiveBind
{
    std::string var;
    int positive;
    int negative;
    KeyUnit* unit;
    bool keep_within;

    TwoKeyActiveBind(const std::string& v, int p, int n, KeyUnit* u, bool kw)
        : var(v)
        , positive(p)
        , negative(n)
        , unit(u)
        , keep_within(kw)
    {
        unit->register_key(positive);
        unit->register_key(negative);
    }

    void set_value_in_table(Table* t, float dt) override
    {
        const auto state = unit->get_state(positive) - unit->get_state(negative);
        t->set(var, keep_within ? input::keep_within(-1.0f, state, 1.0f) : state * dt);
    }
};



KeyBindDefinition::KeyBindDefinition(const std::string& v, int u, int k)
    : var(v)
    , unit(u)
    , key(k)
{
}


std::unique_ptr<ActiveBind> KeyBindDefinition::create(ConnectedUnits* units)
{
    auto* active_unit = units->get_unit(unit);
    ASSERT(active_unit);
    return std::make_unique<KeyActiveBind>(var, key, active_unit->get_key_unit());
}


RelativeAxisBindDefinition::RelativeAxisBindDefinition(const std::string& v, int u, AxisType ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<ActiveBind> RelativeAxisBindDefinition::create(ConnectedUnits* units)
{
    auto* active_unit = units->get_unit(unit);
    ASSERT(active_unit);

    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->get_relative_axis_unit(), false, is_inverted, sensitivity);
}


AbsoluteAxisBindDefinition::AbsoluteAxisBindDefinition(const std::string& v, int u, AxisType ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<ActiveBind> AbsoluteAxisBindDefinition::create(ConnectedUnits* units)
{
    auto* active_unit = units->get_unit(unit);
    ASSERT(active_unit);
    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->get_absolute_axis_unit(), true, is_inverted, sensitivity);
}


RelativeTwoKeyBindDefinition::RelativeTwoKeyBindDefinition(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<ActiveBind> RelativeTwoKeyBindDefinition::create(ConnectedUnits* units)
{
    auto* active_unit = units->get_unit(unit);
    ASSERT(active_unit);
    return std::make_unique<TwoKeyActiveBind>(var, positive, negative, active_unit->get_key_unit(), false);
}


AbsoluteTwoKeyBindDefinition::AbsoluteTwoKeyBindDefinition(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<ActiveBind> AbsoluteTwoKeyBindDefinition::create(ConnectedUnits* units)
{
    auto* active_unit = units->get_unit(unit);
    ASSERT(active_unit);
    return std::make_unique<TwoKeyActiveBind>(var, positive, negative, active_unit->get_key_unit(), true);
}


}
