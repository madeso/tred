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
    RelativeAxisBindDef(const std::string& var, int unit, AxisType type, int target, int axis, bool is_inverted);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
    bool is_inverted;
};


struct AbsoluteAxisBindDef : public BindDef
{
    AbsoluteAxisBindDef(const std::string& var, int unit, AxisType type, int target, int axis, bool is_inverted);

    std::unique_ptr<ActiveBind> Create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
    bool is_inverted;
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


}
