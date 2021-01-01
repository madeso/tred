#include "tred/input/bind.h"

#include "tred/input/table.h"


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


float InputNode::GetValue()
{
    return state;
}


struct InputNodeDef : public NodeDef
{
    std::unique_ptr<Node> Create() override
    {
        return std::make_unique<InputNode>();
    }
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


int ConverterDef::AddVar(const std::string& name, std::unique_ptr<NodeDef>&& output)
{
    using M = decltype(nodes);
    using P = typename M::value_type;

    const auto node_index = static_cast<int>(vars.size());
    nodes.insert(P{name, node_index});
    vars.emplace_back(std::move(output));
    return node_index;
}


int ConverterDef::GetNode(const std::string& name)
{
    const auto found = nodes.find(name);
    if(found == nodes.end())
    {
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
        if(output.node < 0)
        {
            return;
        }
        else
        {
            const auto& node = vars[static_cast<size_t>(output.node)];
            const auto state = node->GetValue();
            SetValue(table, output.var, output.range, state);
        }
    }
}


}
