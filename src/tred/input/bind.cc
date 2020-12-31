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


struct TableOutput : public Output
{
    TableOutput(const std::string& s, Range r)
        : scriptvarname(s)
        , range(r)
    {
    }

    void SetValue(Table* table, float value) override
    {
        table->Set(scriptvarname, KeepWithin(range, value));
    }

    std::string scriptvarname;
    Range range;
};


struct TableOutputDef : public OutputDef
{
    TableOutputDef(const std::string& s, Range r)
        : scriptvarname(s)
        , range(r)
    {
    }

    std::unique_ptr<Output> Create() override
    {
        return std::make_unique<TableOutput>(scriptvarname, range);
    }

    std::string scriptvarname;
    Range range;
};


void ConverterDef::AddOutput(const std::string& name, const std::string& var, Range range)
{
    AddVar(name, std::make_unique<TableOutputDef>(var, range));
}


void ConverterDef::AddVar(const std::string& name, std::unique_ptr<OutputDef>&& output)
{
    using P = typename std::map<std::string, int>::value_type;
    nodes.insert(P{name, static_cast<int>(nodes.size())});
    vars.emplace_back(std::move(output));
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
{
    for(const auto& v: def.vars)
    {
        vars.emplace_back(v->Create());
    }
}


void Converter::Set(int var, Table* table, float value)
{
    if(var < 0)
    {
        return;
    }
    else
    {
        auto& o = vars[static_cast<size_t>(var)];
        o->SetValue(table, value);
    }
}


ValueReciever::ValueReciever(Table* t, Converter* c)
    : table(t)
    , converter(c)
{
}


void ValueReciever::Set(int var, float value)
{
    converter->Set(var, table, value);
}


Bind::Bind(int v, bool i, float s)
    : state(0.0f)
    , var(v)
    , invert(i)
    , scale(s)
{
}


void Bind::SetRawState(float raw)
{
    state = (invert ? -1.0f : 1.0f) * (raw * scale);
}


}
