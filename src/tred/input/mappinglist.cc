#include "tred/input/mappinglist.h"

#include "tred/assert.h"
#include <stdexcept>
#include <fstream>


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


MappingList::MappingList()
{
}


MappingList::MappingList(MappingList&& m)
    : configs(std::move(m.configs))
{
}


void MappingList::operator=(MappingList&& m)
{
    configs = std::move(m.configs);
}

MappingList::~MappingList()
{
}


void MappingList::add(const std::string& name, std::unique_ptr<Mapping>&& config)
{
    ASSERT(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


using BindDefResult = Result<std::unique_ptr<BindDefinition>>;


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


std::optional<std::string> validate_unit(const std::string& action, Mapping* config, int unit)
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


std::optional<std::string> validate_key(const std::string& action, Mapping* config, int unit, int key)
{
    return with_action(action, config->units[Cint_to_sizet(unit)]->validate_key(key));
}


std::optional<std::string> validate_axis(const std::string& action, Mapping* config, int unit, AxisType type, int target, int axis)
{
    return with_action(action, config->units[Cint_to_sizet(unit)]->validate_axis(type, target, axis));
}


BindDefResult create_key_bind_def(Mapping* config, const ActionMap& map, const input::config::KeyBindDefinition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::Action* action = found->second.get();
    if(action->range != Range::within_zero_to_one)
    {
        return with_action(def.action, "Invalid binding key to anything but 0-1");
    }

    if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
    if(auto error = validate_key(def.action, config, def.unit, def.key); error) { return *error; }

    return {std::make_unique<KeyBindDefinition>(action->scriptvarname, def.unit, def.key)};
}


BindDefResult create_axis_bind_def(Mapping* config, const ActionMap& map, const input::config::AxisBindDefinition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::Action* action = found->second.get();
    if(action->range == Range::infinite)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_axis(def.action, config, def.unit, def.type, def.target, def.axis); error) { return *error; }
        return {std::make_unique<RelativeAxisBindDefinition>(action->scriptvarname, def.unit, def.type, def.target, def.axis, def.is_inverted, def.sensitivity)};
    }
    else if(action->range == Range::within_negative_one_to_positive_one)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_axis(def.action, config, def.unit, def.type, def.target, def.axis); error) { return *error; }
        return {std::make_unique<AbsoluteAxisBindDefinition>(action->scriptvarname, def.unit, def.type, def.target, def.axis, def.is_inverted, def.sensitivity)};
    }
    else
    {
        return with_action(def.action, "Axis needs to be bound to either infinite or -1 to +1");
    }
}


BindDefResult create_two_key_bind_def(Mapping* config, const ActionMap& map, const input::config::TwoKeyBindDefinition& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::Action* action = found->second.get();
    if(action->range == Range::infinite)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.negative); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.positive); error) { return *error; }
        return {std::make_unique<RelativeTwoKeyBindDefinition>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else if(action->range == Range::within_negative_one_to_positive_one)
    {
        if(auto error = validate_unit(def.action, config, def.unit); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.negative); error) { return *error; }
        if(auto error = validate_key(def.action, config, def.unit, def.positive); error) { return *error; }
        return {std::make_unique<AbsoluteTwoKeyBindDefinition>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else
    {
        return with_action(def.action, "TwoKeys needs to be bound to either infinite or -1 to +1");
    }
}


std::optional<std::string> load(Mapping* config, const input::config::Mapping& root, const ActionMap& map)
{
    ASSERT(config);

    int joystick_id = 0;

    for(const auto& d: root.units)
    {
        if(d.keyboard)
        {
            config->add(std::make_unique<KeyboardUnitDef>(*d.keyboard));
        }
        else if(d.mouse)
        {
            config->add(std::make_unique<MouseUnitDef>(*d.mouse));
        }
        else if(d.joystick)
        {
            config->add(std::make_unique<JoystickUnitDef>(joystick_id, *d.joystick));
            joystick_id += 1;
        }
        else if(d.gamecontroller)
        {
            config->add(std::make_unique<GamecontrollerUnitDef>(joystick_id, *d.gamecontroller));
            joystick_id += 1;
        }
        else
        {
            DIE("Invalid unit type");
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
            DIE("Invalid bind type");
        }

        #undef ADD_OR_RETURN_DEF
    }

    return std::nullopt;
}


Result<MappingList> load_mapping_list(const input::config::MappingList& root, const ActionMap& map)
{
    MappingList configs;

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<Mapping>(map, d);
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
