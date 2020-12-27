#pragma once

#include <map>
#include <string>
#include <memory>

#include "tred/input-config.h"


namespace input
{

struct KeyConfig;
struct ConnectedUnits;
struct InputActionMap;

struct KeyConfigs
{
    KeyConfigs();

    void Add(const std::string& name, std::shared_ptr<KeyConfig> config);
    std::shared_ptr<KeyConfig> Get(const std::string& name) const;

    std::map<std::string, std::shared_ptr<KeyConfig>> configs;
};


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map);

}  // namespace input
