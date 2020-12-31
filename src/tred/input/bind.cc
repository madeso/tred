#include "tred/input/bind.h"

#include "tred/input/table.h"


namespace input
{


Output::Output(const std::string& s, Range r)
    : scriptvarname(s)
    , range(r)
{
}


void Converter::AddOutput(const std::string& name, const std::string& var, Range range)
{
    using P = typename std::map<std::string, int>::value_type;
    nodes.insert(P{name, static_cast<int>(nodes.size())});
    vars.emplace_back(var, range);
}


int Converter::GetNode(const std::string& name)
{
    const auto found = nodes.find(name);
    if(found == nodes.end())
    {
        return -1;
    }
    return found->second;
}


ValueReciever::ValueReciever(Table* t, const Converter& converter)
    : table(t)
    , vars(converter.vars)
{
}


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


void ValueReciever::Set(int var, float value)
{
    const auto& o = vars[static_cast<size_t>(var)];
    table->Set(o.scriptvarname, KeepWithin(o.range, value));
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
