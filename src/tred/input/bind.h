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


struct table;
struct connected_units;


struct active_bind
{
    virtual ~active_bind() = default;
    virtual void set_value_in_table(table* table, float dt) = 0;
};


struct bind_definition
{
    virtual ~bind_definition() = default;
    virtual std::unique_ptr<active_bind> create(connected_units*) = 0;
};


struct key_bind_definition : public bind_definition
{
    key_bind_definition(const std::string& var, int unit, int key);

    std::unique_ptr<active_bind> create(connected_units* units) override;

    std::string var;
    int unit;
    int key;
};


struct relative_axis_bind_definition : public bind_definition
{
    relative_axis_bind_definition(const std::string& var, int unit, axis_type type, int target, int axis, bool is_inverted, float sensitivity);

    std::unique_ptr<active_bind> create(connected_units* units) override;

    std::string var;
    int unit;
    axis_type type;
    int target;
    int axis;
    bool is_inverted;
    float sensitivity;
};


struct absolute_axis_bind_definition : public bind_definition
{
    absolute_axis_bind_definition(const std::string& var, int unit, axis_type type, int target, int axis, bool is_inverted, float sensitivity);

    std::unique_ptr<active_bind> create(connected_units* units) override;

    std::string var;
    int unit;
    axis_type type;
    int target;
    int axis;
    bool is_inverted;
    float sensitivity;
};


struct relative_two_key_bind_definition : public bind_definition
{
    relative_two_key_bind_definition(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<active_bind> create(connected_units* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


struct absolute_two_key_bind_definition : public bind_definition
{
    absolute_two_key_bind_definition(const std::string& var, int unit, int negative, int positive);

    std::unique_ptr<active_bind> create(connected_units* units) override;

    std::string var;
    int unit;
    int negative;
    int positive;
};


template<typename T>
struct bind_map
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
        return found->second;
    }

    std::map<T, float> binds;
};


}
