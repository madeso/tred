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


struct ValueReciever
{
    ValueReciever(Table* table, Converter* converter);

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
struct BindMap
{
    explicit BindMap(const std::vector<BindDef<T>>& src, Converter*)
    {
        for(const auto& b: src)
        {
            using P = typename std::map<T, Bind>::value_type;
            binds.insert(P{b.type, Bind{b.var, b.invert, b.scale}});
        }
    }

    void Recieve(ValueReciever* r)
    {
        for(const auto& b: binds)
        {
            r->converter->Set(b.second.var, r->table, b.second.state);
        }
    }

    void SetRaw(const T& t, float state)
    {
        auto found = binds.find(t);
        if (found != binds.end())
        {
            found->second.SetRawState(state);
        }
    }

    std::map<T, Bind> binds;
};

}
