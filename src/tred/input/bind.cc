#include "tred/input/bind.h"

#include <cassert>

#include "tred/input/table.h"
#include "tred/input/activeunit.h"
#include "tred/input/connectedunits.h"
#include "tred/log.h"


namespace input
{


float KeepWithin(float mi, float v, float ma)
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
        unit->RegisterKey(button);
    }

    void SetValueInTable(Table* t, float)
    {
        const auto state = unit->GetState(button);
        t->Set(var, KeepWithin(0.0f, state, 1.0f));
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
        unit->RegisterAxis(type, target, axis);
    }

    void SetValueInTable(Table* t, float dt)
    {
        const auto invert_scale = is_inverted ? -1.0f : 1.0f;
        const auto state = unit->GetState(type, target, axis, keep_within ? 1.0f : dt) * invert_scale;
        t->Set(var, keep_within ? KeepWithin(-1.0f, state, 1.0f) : state * sensitivity);
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
        unit->RegisterKey(positive);
        unit->RegisterKey(negative);
    }

    void SetValueInTable(Table* t, float dt)
    {
        const auto state = unit->GetState(positive) - unit->GetState(negative);
        t->Set(var, keep_within ? KeepWithin(-1.0f, state, 1.0f) : state * dt);
    }
};



KeyBindDef::KeyBindDef(const std::string& v, int u, int k)
    : var(v)
    , unit(u)
    , key(k)
{
}


std::unique_ptr<ActiveBind> KeyBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);
    return std::make_unique<KeyActiveBind>(var, key, active_unit->GetKeyUnit());
}


RelativeAxisBindDef::RelativeAxisBindDef(const std::string& v, int u, AxisType ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<ActiveBind> RelativeAxisBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);

    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->GetRelativeAxisUnit(), false, is_inverted, sensitivity);
}


AbsoluteAxisBindDef::AbsoluteAxisBindDef(const std::string& v, int u, AxisType ty, int ta, int ax, bool ii, float s)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
    , is_inverted(ii)
    , sensitivity(s)
{
}


std::unique_ptr<ActiveBind> AbsoluteAxisBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);
    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->GetAbsoluteAxisUnit(), true, is_inverted, sensitivity);
}


RelativeTwoKeyBindDef::RelativeTwoKeyBindDef(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<ActiveBind> RelativeTwoKeyBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);
    return std::make_unique<TwoKeyActiveBind>(var, positive, negative, active_unit->GetKeyUnit(), false);
}


AbsoluteTwoKeyBindDef::AbsoluteTwoKeyBindDef(const std::string& v, int u, int ne, int po)
    : var(v)
    , unit(u)
    , negative(ne)
    , positive(po)
{
}


std::unique_ptr<ActiveBind> AbsoluteTwoKeyBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);
    return std::make_unique<TwoKeyActiveBind>(var, positive, negative, active_unit->GetKeyUnit(), true);
}


}
