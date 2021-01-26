#include "tred/input/mappinglist.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"

#include "tred/log.h"

#include "tred/input/mapping.h"
#include "tred/input/keyboarddef.h"
#include "tred/input/joystickdef.h"
#include "tred/input/mousedef.h"
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


void MappingList::Add(const std::string& name, std::unique_ptr<Mapping>&& config)
{
    assert(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


using BindDefResult = Result<std::unique_ptr<BindDef>>;


BindDefResult CreateKeyBindDef(const InputActionMap& map, const input::config::KeyBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::InputAction* action = found->second.get();
    if(action->range != Range::WithinZeroToOne)
    {
        return fmt::format("Invalid range bind");
    }

    return {std::make_unique<KeyBindDef>(action->scriptvarname, def.unit, def.key)};
}


BindDefResult CreateAxisBindDef(const InputActionMap& map, const input::config::AxisBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::InputAction* action = found->second.get();
    if(action->range == Range::Infinite)
    {
        return {std::make_unique<RelativeAxisBindDef>(action->scriptvarname, def.unit, def.type, def.target, def.axis)};
    }
    else if(action->range == Range::WithinNegativeOneToPositiveOne)
    {
        return {std::make_unique<AbsoluteAxisBindDef>(action->scriptvarname, def.unit, def.type, def.target, def.axis)};
    }
    else
    {
        return fmt::format("Invalid range bind");
    }
}


BindDefResult CreateTwoKeyBindDef(const InputActionMap& map, const input::config::TwoKeyBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        return fmt::format("Unknown action {}", def.action);
    }
    input::InputAction* action = found->second.get();
    if(action->range == Range::Infinite)
    {
        return {std::make_unique<RelativeTwoKeyBindDef>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else if(action->range == Range::WithinNegativeOneToPositiveOne)
    {
        return {std::make_unique<AbsoluteTwoKeyBindDef>(action->scriptvarname, def.unit, def.negative, def.positive)};
    }
    else
    {
        return fmt::format("Invalid range bind");
    }
}


std::optional<std::string> Load(Mapping* config, const input::config::Mapping& root, const InputActionMap& map)
{
    assert(config);

    int joystick_id = 0;

    for(const auto& d: root.units)
    {
        if(d.keyboard)
        {
            config->Add(std::make_unique<KeyboardDef>(*d.keyboard));
        }
        else if(d.mouse)
        {
            config->Add(std::make_unique<MouseDef>(*d.mouse));
        }
        else if(d.joystick)
        {
            config->Add(std::make_unique<JoystickDef>(joystick_id, *d.joystick));
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
                config->Add(std::move(def.value()));\
            }\
            else\
            {\
                return def.error();\
            }

        if(d.key)
        {
            ADD_OR_RETURN_DEF(CreateKeyBindDef(map, *d.key));
        }
        else if(d.axis)
        {
            ADD_OR_RETURN_DEF(CreateAxisBindDef(map, *d.axis));
        }
        else if(d.twokey)
        {
            ADD_OR_RETURN_DEF(CreateTwoKeyBindDef(map, *d.twokey));
        }
        else
        {
            assert(false && "Unhandled bind type");
        }

        #undef ADD_OR_RETURN_DEF
    }

    return std::nullopt;
}


Result<MappingList> LoadMappingList(const input::config::MappingList& root, const InputActionMap& map)
{
    MappingList configs;

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<Mapping>(map, d);
        auto error = Load(config.get(), d, map);
        if(error)
        {
            return *error;
        }
        configs.Add(name, std::move(config));
    }

    return configs;
}


}  // namespace input
