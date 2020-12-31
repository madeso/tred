#include "tred/input/keyconfigs.h"

#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"


#include "tred/input/keyconfig.h"
#include "tred/input/keyboarddef.h"
#include "tred/input/joystickdef.h"
#include "tred/input/mousedef.h"
#include "tred/input/config.h"


namespace input
{


KeyConfigs::KeyConfigs()
{
}

KeyConfigs::~KeyConfigs()
{
}


void KeyConfigs::Add(const std::string& name, std::unique_ptr<KeyConfig>&& config)
{
    assert(config);
    configs.insert(std::make_pair(name, std::move(config)));
}


KeyConfig& KeyConfigs::Get(const std::string& name) const
{
    auto res = configs.find(name);
    if (res == configs.end())
    {
        throw fmt::format("Unable to find config: {}", name);
    }

    assert(res->second);
    return *res->second;
}


void Load(KeyConfig* config, const input::config::Config& root)
{
    assert(config);

    for (const auto& d: root.keyboards)
    {
        config->Add(std::make_unique<KeyboardDef>(d, &config->converter));
    }

    for (const auto& d: root.mouses)
    {
        config->Add(std::make_unique<MouseDef>(d, &config->converter));
    }

    for (const auto& d: root.joysticks)
    {
        config->Add(std::make_unique<JoystickDef>(d, &config->converter));
    }
}


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map)
{
    assert(configs);

    for (const auto& d: root)
    {
        const std::string name = d.name;
        auto config = std::make_unique<KeyConfig>(map);
        Load(config.get(), d);
        configs->Add(name, std::move(config));
    }
}


}  // namespace input
