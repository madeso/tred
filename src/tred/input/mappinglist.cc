#include "tred/input/mappinglist.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/cint.h"

#include "tred/input/mapping.h"
#include "tred/input/unitdef.keyboard.h"
#include "tred/input/unitdef.joystick.h"
#include "tred/input/unitdef.mouse.h"
#include "tred/input/unitdef.gamecontroller.h"
#include "tred/input/config.h"
#include "tred/input/action.h"
#include "tred/input/actionmap.h"


namespace input
{


mapping_list::mapping_list()
{
}


mapping_list::mapping_list(mapping_list&& m)
    : configs(std::move(m.configs))
{
}


void mapping_list::operator=(mapping_list&& m)
{
    configs = std::move(m.configs);
}

mapping_list::~mapping_list()
{
}


void mapping_list::add(const std::string& name, std::unique_ptr<mapping>&& config)
{
    assert(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


using BindDefResult = result<std::unique_ptr<bind_definition>>;


std::string with_action(const std::string&action, const std::string& error)
{
    return fmt::format("{} (action: {})", error, action);
}


std::string with_action(const std::string&action, const char* const error)
{
    return with_action(action, std::string{error});
}


std::optional<std::string> with_action(const std::string&action, std::optional<std::string> result)
{
    if(result)
    {
        return with_action(action, *result);
    }
    else
    {
        return std::nullopt;
    }
}


std::optional<std::string> validate_unit(const std::string& action, mapping* config, int unit)
{
    if(unit < 0)
    {
        return with_action(action, "Negative units are invalid");
    }

    const auto size = Csizet_to_int(config->units.size());
    if(unit >= size)
    {
        return with_action(action, fmt::format("Invalid unit {}", unit));
    }

    return std::nullopt;
}


std::optional<std::string> validate_key(const std::string& action, mapping* config, int unit, int key)
{
    return with_action(action, config->units[Cint_to_sizet(unit)]->validate_key(key));
}


std::optional<std::string> validate_axis(const std::string& action, mapping* config, int unit, axis_type type, int target, int axis)
{
    return with_action(action, config->units[Cint_to_sizet(unit)]->validate_axis(type, target, axis));
}


BindDefResult create_key_bind_def(mapping* config, const input_action_map& map, const input::config::key_bind_definition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::input_action* action = found->second.get();
    if(action->range != range::within_zero_to_one)
    {
        return with_action(def.action, "Invalid binding key to anything but 0-1");
    }

    if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
    if(auto error = validate_key(def.action, config, def.unit, def.key); error) { return *error; }

    return {std::make_unique<key_bind_definition>(action->scriptvarname, def.unit, def.key)};
}


BindDefResult create_axis_bind_def(mapping* config, const input_action_map& map, const input::config::axis_bind_definition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::input_action* action = found->second.get();
    if(action->range == range::infinite)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_axis(def.action, config, def.unit, def.type, def.target, def.axis); error) { return *error; }
        return {std::make_unique<relative_axis_bind_definition>(action->scriptvarname, def.unit, def.type, def.target, def.axis, def.is_inverted, def.sensitivity)};
    }
    else if(action->range == range::within_negative_one_to_positive_one)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_axis(def.action, config, def.unit, def.type, def.target, def.axis); error) { return *error; }
        return {std::make_unique<absolute_axis_bind_definition>(action->scriptvarname, def.unit, def.type, def.target, def.axis, def.is_inverted, def.sensitivity)};
    }
    else
    {
        return with_action(def.action, "Axis needs to be bound to either infinite or -1 to +1");
    }
}


BindDefResult create_two_key_bind_def(mapping* config, const input_action_map& map, const input::config::two_key_bind_definition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::input_action* action = found->second.get();
    if(action->range == range::infinite)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.negative); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.positive); error) { return *error; }
        return {std::make_unique<relative_two_key_bind_definition>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else if(action->range == range::within_negative_one_to_positive_one)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.negative); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.positive); error) { return *error; }
        return {std::make_unique<absolute_two_key_bind_definition>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else
    {
        return with_action(def.action, "TwoKeys needs to be bound to either infinite or -1 to +1");
    }
}


std::optional<std::string> load(mapping* config, const input::config::mapping& root, const input_action_map& map)
{
    assert(config);

    int joystick_id = 0;

    for(const auto& d: root.units)
    {
        if(d.keyboard)
        {
            config->add(std::make_unique<keyboard_definition>(*d.keyboard));
        }
        else if(d.mouse)
        {
            config->add(std::make_unique<mouse_definition>(*d.mouse));
        }
        else if(d.joystick)
        {
            config->add(std::make_unique<joystick_definition>(joystick_id, *d.joystick));
            joystick_id += 1;
        }
        else if(d.gamecontroller)
        {
            config->add(std::make_unique<gamecontroller_definition>(joystick_id, *d.gamecontroller));
            joystick_id += 1;
        }
        else
        {
            assert(false && "Unhandled unit type");
        }
    }

    for(const auto& d: root.binds)
    {
        #define ADD_OR_RETURN_DEF(DEF) \
            auto def = DEF;\
            if(def)\
            {\
                config->add(std::move(*def.value));\
            }\
            else\
            {\
                return fmt::format("{} (mapping: {})", def.get_error(), root.name);\
            }

        if(d.key)
        {
            ADD_OR_RETURN_DEF(create_key_bind_def(config, map, *d.key));
        }
        else if(d.axis)
        {
            ADD_OR_RETURN_DEF(create_axis_bind_def(config, map, *d.axis));
        }
        else if(d.twokey)
        {
            ADD_OR_RETURN_DEF(create_two_key_bind_def(config, map, *d.twokey));
        }
        else
        {
            assert(false && "Unhandled bind type");
        }

        #undef ADD_OR_RETURN_DEF
    }

    return std::nullopt;
}


result<mapping_list> load_mapping_list(const input::config::mapping_list& root, const input_action_map& map)
{
    mapping_list configs;

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<mapping>(map, d);
        auto error = load(config.get(), d, map);
        if(error)
        {
            return *error;
        }
        configs.add(name, std::move(config));
    }

    return configs;
}


}  // namespace input
