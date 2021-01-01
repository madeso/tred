#include "tred/input/bind.h"

#include <cassert>

#include "tred/input/table.h"
#include "tred/log.h"


namespace input
{



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


}
