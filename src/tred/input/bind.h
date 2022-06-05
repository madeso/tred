#pragma once


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
    virtual void set_value_in_table(Table* table, float dt) = 0;
};


struct BindDefinition
{
    virtual ~BindDefinition() = default;
    virtual std::unique_ptr<ActiveBind> create(ConnectedUnits*) = 0;
};


struct KeyBindDefinition : public BindDefinition
{
    KeyBindDefinition(const std::string& var, int unit, int key);

    std::unique_ptr<ActiveBind> create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int key;
};


struct RelativeAxisBindDefinition : public BindDefinition
{
    RelativeAxisBindDefinition(const std::string& var, int unit, AxisType type, int target, int axis, bool is_inverted, float sensitivity);

    std::unique_ptr<ActiveBind> create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
    bool is_inverted;
    float sensitivity;
};


struct AbsoluteAxisBindDefinition : public BindDefinition
{
    AbsoluteAxisBindDefinition(const std::string& var, int unit, AxisType type, int target, int axis, bool is_inverted, float sensitivity);

    std::unique_ptr<ActiveBind> create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    AxisType type;
    int target;
    int axis;
    bool is_inverted;
    float sensitivity;
};


struct RelativeTwoKeyBindDefinition : public BindDefinition
{
    RelativeTwoKeyBindDefinition(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<ActiveBind> create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


struct AbsoluteTwoKeyBindDefinition : public BindDefinition
{
    AbsoluteTwoKeyBindDefinition(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<ActiveBind> create(ConnectedUnits* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


template<typename T>
struct BindMap
{
    void add(const T& t, float state=0.0f)
    {
        using M = decltype(binds);
        using P = typename M::value_type;
        binds.insert(P{t, state});
    }

    void set_raw(const T& t, float state)
    {
        auto found = binds.find(t);
        if (found != binds.end())
        {
            found->second = state;
        }
    }

    float get_raw(const T& t) const
    {
        auto found = binds.find(t);
        if(found == binds.end())
        {
            return 0.0f;
        }
        else
        {
            return found->second;
        }
    }

    std::unordered_map<T, float> binds;
};


}
