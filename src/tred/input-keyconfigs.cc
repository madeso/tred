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
        throw fmt::format("Unable to find config: {}", name);
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


void Load(KeyConfig* config, const input::config::Config& root, const InputActionMap& map)
{
    assert(config);

    for (const auto& d: root.keyboards)
    {
        config->Add(std::make_shared<KeyboardDef>(d, map));
    }

    for (const auto& d: root.mouses)
    {
        config->Add(std::make_shared<MouseDef>(d, map));
    }

    for (const auto& d: root.joysticks)
    {
        config->Add(std::make_shared<JoystickDef>(d, map));
    }
}


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map)
{
    assert(configs);

    for (const auto& d: root)
    {
        const std::string name = d.name;
        std::shared_ptr<KeyConfig> config(new KeyConfig());
        Load(config.get(), d, map);
        configs->Add(name, config);
    }
}


}  // namespace input
