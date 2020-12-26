#pragma once

#include <map>
#include <string>
#include <memory>


namespace input
{

namespace config
{
    struct KeyConfigs;
}

struct KeyConfig;
struct ConnectedUnits;
struct InputActionMap;

struct KeyConfigs
{
    KeyConfigs();

    void Add(const std::string& name, std::shared_ptr<KeyConfig> config);
    std::shared_ptr<KeyConfig> Get(const std::string& name) const;

    void BeginAutoDetect();
    void AbortAutoDetect();

    std::shared_ptr<ConnectedUnits> GetFirstAutoDetectedConfig() const;

    std::map<std::string, std::shared_ptr<KeyConfig>> configs;
};


void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map);

}  // namespace input
