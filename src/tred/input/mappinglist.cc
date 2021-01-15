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

MappingList::~MappingList()
{
}


void MappingList::Add(const std::string& name, std::unique_ptr<Mapping>&& config)
{
    assert(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


std::unique_ptr<BindDef> CreateBindDef(const InputActionMap& map, const input::config::KeyBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        LOG_ERROR("Unknown action {}", def.action);
        return nullptr;
    }
    input::InputAction* action = found->second.get();
    if(action->range != Range::WithinZeroToOne)
    {
        LOG_ERROR("Invalid range bind");
        return nullptr;
    }

    return std::make_unique<KeyBindDef>(action->scriptvarname, def.unit, def.key);
}


std::unique_ptr<BindDef> CreateBindDef(const InputActionMap& map, const input::config::AxisBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        LOG_ERROR("Unknown action {}", def.action);
        return nullptr;
    }
    input::InputAction* action = found->second.get();
    if(action->range == Range::Infinite)
    {
        return std::make_unique<RelativeAxisBindDef>(action->scriptvarname, def.unit, def.type, def.target, def.axis);
    }
    else if(action->range == Range::WithinNegativeOneToPositiveOne)
    {
        return std::make_unique<AbsoluteAxisBindDef>(action->scriptvarname, def.unit, def.type, def.target, def.axis);
    }
    else
    {
        LOG_ERROR("Invalid range bind");
        return nullptr;
    }
}


std::unique_ptr<BindDef> CreateBindDef(const InputActionMap& map, const input::config::TwoKeyBindDef& def)
{
    const auto found = map.actions.find(def.action);
    if(found == map.actions.end())
    {
        LOG_ERROR("Unknown action {}", def.action);
        return nullptr;
    }
    input::InputAction* action = found->second.get();
    if(action->range == Range::Infinite)
    {
        return std::make_unique<RelativeTwoKeyBindDef>(action->scriptvarname, def.unit, def.negative, def.positive);
    }
    else if(action->range == Range::WithinNegativeOneToPositiveOne)
    {
        return std::make_unique<AbsoluteTwoKeyBindDef>(action->scriptvarname, def.unit, def.negative, def.positive);
    }
    else
    {
        LOG_ERROR("Invalid range bind");
        return nullptr;
    }
}


void Load(Mapping* config, const input::config::Mapping& root, const InputActionMap& map)
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
        if(d.key)
        {
            config->Add(CreateBindDef(map, *d.key));
        }
        else if(d.axis)
        {
            config->Add(CreateBindDef(map, *d.axis));
        }
        else if(d.twokey)
        {
            config->Add(CreateBindDef(map, *d.twokey));
        }
        else
        {
            assert(false && "Unhandled bind type");
        }
    }
}


void Load(MappingList* configs, const input::config::MappingList& root, const InputActionMap& map)
{
    assert(configs);

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<Mapping>(map, d);
        Load(config.get(), d, map);
        configs->Add(name, std::move(config));
    }
}


}  // namespace input
