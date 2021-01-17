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


#if 0
float KeepWithin(Range r, float v)
{
    switch(r)
    {
        case Range::WithinNegativeOneToPositiveOne: return KeepWithin(-1.0f, v, 1.0f);
        case Range::WithinZeroToOne: return KeepWithin(0.0f, v, 1.0f);
        case Range::Infinite:
        default:
            return v;
    }
}
#endif


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

    AxisActiveBind(const std::string& v, AxisType ty, int ta, int ax, AxisUnit* u, bool kw)
        : var(v)
        , type(ty)
        , target(ta)
        , axis(ax)
        , unit(u)
        , keep_within(kw)
    {
        unit->RegisterAxis(type, target, axis);
    }

    void SetValueInTable(Table* t, float dt)
    {
        const auto state = unit->GetState(type, target, axis, keep_within ? 1.0f : dt);
        t->Set(var, keep_within ? KeepWithin(-1.0f, state, 1.0f) : state);
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


RelativeAxisBindDef::RelativeAxisBindDef(const std::string& v, int u, AxisType ty, int ta, int ax)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
{
}


std::unique_ptr<ActiveBind> RelativeAxisBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);

    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->GetRelativeAxisUnit(), false);
}


AbsoluteAxisBindDef::AbsoluteAxisBindDef(const std::string& v, int u, AxisType ty, int ta, int ax)
    : var(v)
    , unit(u)
    , type(ty)
    , target(ta)
    , axis(ax)
{
}


std::unique_ptr<ActiveBind> AbsoluteAxisBindDef::Create(ConnectedUnits* units)
{
    auto* active_unit = units->GetUnit(unit);
    assert(active_unit);
    return std::make_unique<AxisActiveBind>(var, type, target, axis, active_unit->GetAbsoluteAxisUnit(), true);
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


#if 0
float KeepWithin(float mi, float v, float ma)
{
    if (v < mi) { return mi; }
    else if (v > ma) { return ma; }
    else { return v; }
}


float KeepWithin(Range r, float v)
{
    switch(r)
    {
        case Range::WithinNegativeOneToPositiveOne: return KeepWithin(-1.0f, v, 1.0f);
        case Range::WithinZeroToOne: return KeepWithin(0.0f, v, 1.0f);
        case Range::Infinite:
        default:
            return v;
    }
}


void SetValue(Table* table, const std::string& scriptvarname, Range range, float value)
{
    table->Set(scriptvarname, KeepWithin(range, value));
}


InputNode::InputNode()
    : state(0.0f)
{
}


float InputNode::GetValue(Converter*)
{
    return state;
}


struct InputNodeDef : public NodeDef
{
    bool IsInput() override
    {
        return true;
    }

    std::unique_ptr<Node> Create() override
    {
        return std::make_unique<InputNode>();
    }
};


struct TwoButtonConverter : Node
{
    TwoButtonConverter(int p, int n)
        : positive(p)
        , negative(n)
    {
    }

    float GetValue(Converter* converter) override
    {
        return converter->GetState(positive) - converter->GetState(negative);
    }

    int positive;
    int negative;
};


struct TwoButtonConverterDef : public NodeDef
{
    TwoButtonConverterDef(int p, int n)
        : positive(p)
        , negative(n)
    {
    }

    bool IsInput() override
    {
        return false;
    }

    std::unique_ptr<Node> Create() override
    {
        return std::make_unique<TwoButtonConverter>(positive, negative);
    }

    int positive;
    int negative;
};


Output::Output(int n, const std::string& v, Range r)
        : node(n)
        , var(v)
        , range(r)
{
}


void ConverterDef::AddOutput(const std::string& name, const std::string& var, Range range)
{
    const auto node = AddVar(name, std::make_unique<InputNodeDef>());
    outputs.emplace_back(node, var, range);
}


void ConverterDef::AddTwoButton(const std::string& name)
{
    const auto pos = AddVar(name+"+", std::make_unique<InputNodeDef>());
    const auto neg = AddVar(name+"-", std::make_unique<InputNodeDef>());
    AddVar(name, std::make_unique<TwoButtonConverterDef>(pos, neg));
}


int ConverterDef::AddVar(const std::string& name, std::unique_ptr<NodeDef>&& output)
{
    assert(is_adding);

    using M = decltype(nodes);
    using P = typename M::value_type;

    auto found = nodes.find(name);
    if(found != nodes.end())
    {
        // if the node exists, replace the old node with the new node
        const auto node_index = found->second;
        auto& node = vars[static_cast<std::size_t>(node_index)];
        node = std::move(output);
        return node_index;
    }
    else
    {
        const auto node_index = static_cast<int>(vars.size());
        nodes.insert(P{name, node_index});
        vars.emplace_back(std::move(output));
        return node_index;
    }
}


int ConverterDef::GetNode(const std::string& name)
{
    is_adding = false;

    const auto found = nodes.find(name);
    if(found == nodes.end())
    {
        LOG_WARNING("Unable to find a node named '{}'", name);
        return -1;
    }
    int index = found->second;
    auto& node = vars[static_cast<size_t>(index)];
    if(node->IsInput() == false)
    {
        LOG_WARNING("Node '{}' is not marked as input", name);
        return -1;
    }
    return found->second;
}


Converter::Converter(const ConverterDef& def)
    : outputs(def.outputs)
{
    for(const auto& v: def.vars)
    {
        vars.emplace_back(v->Create());
    }
}


void Converter::SetRawState(int var, float value)
{
    if(var < 0)
    {
        return;
    }
    else
    {
        auto& o = vars[static_cast<size_t>(var)];
        auto* input = static_cast<InputNode*>(o.get());
        input->state = value;
    }
}

void Converter::SetTable(Table* table)
{
    for(const auto& output: outputs)
    {
        const auto state = GetState(output.node);
        SetValue(table, output.var, output.range, state);
    }
}


float Converter::GetState(int node_index)
{
    if(node_index < 0)
    {
        return 0.0f;
    }
    else
    {
        const auto& node = vars[static_cast<size_t>(node_index)];
        const auto state = node->GetValue(this);
        return state;
    }
}
#endif

}
