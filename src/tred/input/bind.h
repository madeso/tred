#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>

#include "tred/input/range.h"


namespace input
{


struct Table;


struct Output
{
    virtual ~Output() = default;

    virtual void SetValue(Table* table, float value) = 0;
};


struct OutputDef
{
    virtual ~OutputDef() = default;

    virtual std::unique_ptr<Output> Create() = 0;
};


struct ConverterDef
{
    void AddOutput(const std::string& name, const std::string& var, Range range);
    void AddVar(const std::string& name, std::unique_ptr<OutputDef>&& output);

    int GetNode(const std::string& name);

    std::vector<std::unique_ptr<OutputDef>> vars;
    std::map<std::string, int> nodes;
};


struct Converter
{
    explicit Converter(const ConverterDef& converter);
    
    void Set(int var, Table* table, float value);

    std::vector<std::unique_ptr<Output>> vars;
};


// could probably be replaced with a std::function as all it ever does
// is call Converter::Set
struct ValueReciever
{
    ValueReciever(Table* table, Converter* converter);

    void Set(int var, float value);

    Table* table;
    Converter* converter;
};


template<typename T>
struct BindDef
{
    template<typename TT> BindDef(const std::string& v, const T& t, const TT& tt, ConverterDef* converter)
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
