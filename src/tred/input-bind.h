#pragma once

#include <string>
#include <map>
#include <vector>

#include "tred/input-range.h"


namespace input
{


struct Table;


struct Output
{
    Output(const std::string& scriptvarname, Range range);

    std::string scriptvarname;
    Range range;
};


struct Converter
{
    void AddOutput(const std::string& name, const std::string& var, Range range);

    int GetNode(const std::string& name);

    std::vector<Output> vars;
    std::map<std::string, int> nodes;
};


struct ValueReciever
{
    ValueReciever(Table* table, const Converter& converter);

    void Set(int var, float value);

    Table* table;
    std::vector<Output> vars;
};


template<typename T>
struct BindDef
{
    template<typename TT> BindDef(const std::string& v, const T& t, const TT& tt, Converter* converter)
        : var(converter->GetNode(v))
        , type(t)
        , invert(tt.invert)
        , scale(tt.scale)
    {
    }

    int var;
    T type;
    bool invert;
    float scale;
};


struct Bind
{
    Bind(int v, bool i, float s);

    float state;
    int var;
    bool invert;
    float scale;

    void SetRawState(float raw);
};


template<typename T>
std::map<T, Bind> ConvertToBindMap(const std::vector<BindDef<T>>& binds)
{
    auto r = std::map<T, Bind>{};

    for(const auto& b: binds)
    {
        using P = typename std::map<T, Bind>::value_type;
        r.insert(P{b.type, Bind{b.var, b.invert, b.scale}});
    }

    return r;
}


template<typename T>
void CallRecieve(const std::map<T, Bind>& binds, ValueReciever* reciever)
{
    for(const auto& b: binds)
    {
        reciever->Set(b.second.var, b.second.state);
    }
}

}
