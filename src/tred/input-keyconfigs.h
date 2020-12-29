#pragma once

#include <map>
#include <string>
#include <memory>

#include "tred/input-config.h"


namespace input
{

struct KeyConfig;
struct InputActionMap;

struct KeyConfigs
{
    KeyConfigs();
    ~KeyConfigs();

    void Add(const std::string& name, std::unique_ptr<KeyConfig>&& config);

    KeyConfig& Get(const std::string& name) const;

    std::map<std::string, std::unique_ptr<KeyConfig>> configs;
};


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map);

}  // namespace input
