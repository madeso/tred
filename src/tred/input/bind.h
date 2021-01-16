#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>


#include "tred/input/axis.h"
#include "tred/input/axistype.h"
#include "tred/input/bindunit.h"


namespace input
{


struct Table;
struct ConnectedUnits;


struct ActiveBind
{
    virtual ~ActiveBind() = default;
    virtual void SetValueInTable(Table* table, float dt) = 0;
};


struct BindDef
{
    virtual ~BindDef() = default;
    virtual std::unique_ptr<ActiveBind> Create(ConnectedUnits*) = 0;
};


struct KeyBindDef : public BindDef
{
    KeyBindDef(const std::string& var, int unit, int key);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int key;
};


struct RelativeAxisBindDef : public BindDef
{
    RelativeAxisBindDef(const std::string& var, int unit, AxisType type, int target, int axis);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
};


struct AbsoluteAxisBindDef : public BindDef
{
    AbsoluteAxisBindDef(const std::string& var, int unit, AxisType type, int target, int axis);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
};


struct RelativeTwoKeyBindDef : public BindDef
{
    RelativeTwoKeyBindDef(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


struct AbsoluteTwoKeyBindDef : public BindDef
{
    AbsoluteTwoKeyBindDef(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


template<typename T>
struct BindMap
{
    void Add(const T& t, float state=0.0f)
    {
        using M = decltype(binds);
        using P = typename M::value_type;
        binds.insert(P{t, state});
    }

    void SetRaw(const T& t, float state)
    {
        auto found = binds.find(t);
        if (found != binds.end())
        {
            found->second = state;
        }
    }

    float GetRaw(const T& t) const
    {
        auto found = binds.find(t);
        return found->second;
    }

    std::map<T, float> binds;
};


#if 0
struct Node
{
    virtual ~Node() = default;

    virtual float GetValue(Converter*) = 0;
};


struct InputNode : public Node
{
    InputNode();

    float state;

    float GetValue(Converter*) override;
};


struct NodeDef
{
    virtual ~NodeDef() = default;

    virtual bool IsInput() = 0;

    virtual std::unique_ptr<Node> Create() = 0;
};


struct Output
{
    Output(int node, const std::string& var, Range range);

    int node;
    std::string var;
    Range range;
};


struct ConverterDef
{
    void AddOutput(const std::string& name, const std::string& var, Range range);
    void AddTwoButton(const std::string& name);

    int AddVar(const std::string& name, std::unique_ptr<NodeDef>&& Node);

    int GetNode(const std::string& name);

    std::vector<std::unique_ptr<NodeDef>> vars;
    std::map<std::string, int> nodes;
    std::vector<Output> outputs;
    bool is_adding = true;
};


struct Converter
{
    explicit Converter(const ConverterDef& converter);

    void SetRawState(int var, float value);
    void SetTable(Table* table);

    float GetState(int node);

    std::vector<std::unique_ptr<Node>> vars;
    std::vector<Output> outputs;
};


template<typename T>
struct BindDef
{
    template<typename TT> BindDef(const std::string& v, const T& t, const TT& tt, ConverterDef* converter)
        : node(converter->GetNode(v))
        , type(t)
        , invert(tt.invert)
        , scale(tt.scale)
    {
    }

    int node;
    T type;
    bool invert;
    float scale;
};


template<typename T>
struct BindMap
{
    BindMap(const std::vector<BindDef<T>>& src, Converter* c)
        : converter(c)
    {
        for(const auto& b: src)
        {
            using M = decltype(binds);
            using P = typename M::value_type;
            binds.insert(P{b.type, b.node});
        }
    }

    void SetRaw(const T& t, float state)
    {
        auto found = binds.find(t);
        if (found != binds.end())
        {
            converter->SetRawState(found->second, state);
        }
    }

    Converter* converter;
    std::map<T, int> binds;
};
#endif

}
