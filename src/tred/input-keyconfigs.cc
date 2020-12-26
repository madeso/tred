#include "tred/input-keyconfigs.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"


#include "tred/input-keyconfig.h"
#include "tred/input-keyboarddef.h"
#include "tred/input-joystickdef.h"
#include "tred/input-mousedef.h"
#include "tred/input-config.h"


namespace input
{


KeyConfigs::KeyConfigs()
{
}


void KeyConfigs::Add(const std::string& name, std::shared_ptr<KeyConfig> config)
{
    assert(config);
    configs.insert(std::make_pair(name, config));
}


std::shared_ptr<KeyConfig> KeyConfigs::Get(const std::string& name) const
{
    auto res = configs.find(name);
    if (res == configs.end())
    {
        const std::string error = fmt::format("Unable to find config: {}", name);
        throw error;
    }

    assert(res->second);
    return res->second;
}


void KeyConfigs::BeginAutoDetect()
{
    assert(false && "Not implemented yet");
}


void KeyConfigs::AbortAutoDetect()
{
    assert(false && "Not implemented yet");
}


std::shared_ptr<ConnectedUnits> KeyConfigs::GetFirstAutoDetectedConfig() const
{
    assert(false && "Not implemented yet");
    std::shared_ptr<ConnectedUnits> dummy;
    return dummy;
}


std::shared_ptr<UnitDef> CreateUnit(const input::config::UnitInterface& type, const InputActionMap& map)
{
    if (type.keyboard.has_value())
    {
        std::shared_ptr<UnitDef> def;
        def.reset(new KeyboardDef(*type.keyboard, map));
        return def;
    }
    else if (type.mouse.has_value())
    {
        std::shared_ptr<UnitDef> def;
        def.reset(new MouseDef(*type.mouse, map));
        return def;
    }
    else if (type.joystick.has_value())
    {
        std::shared_ptr<UnitDef> def;
        def.reset(new JoystickDef(*type.joystick, map));
        return def;
    }
    else
    {
        throw std::logic_error("Unknown unit definition");
    }
}


void Load(KeyConfig* config, const input::config::Config& root, const InputActionMap& map)
{
    assert(config);

    for (const auto& d: root.units)
    {
        auto def = CreateUnit(d, map);
        assert(def);
        config->Add(def);
    }
}


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map)
{
    assert(configs);

    for (const auto& d: root.configs)
    {
        const std::string name = d.name;
        std::shared_ptr<KeyConfig> config(new KeyConfig());
        Load(config.get(), d, map);
        configs->Add(name, config);
    }
}


}  // namespace input
