/** @file
Classes for input handling.
 */

#pragma once
#include <map>
#include <string>
#include <memory>


namespace input
{
struct KeyConfig;
struct ConnectedUnits;
struct InputActionMap;

/** Contains a list of configurations.
 */
struct KeyConfigs
{
    /** Constructor.
   */
    KeyConfigs();

    /** Add a configuration.
  @param name the name of the configuration
  @param config the configuration to add
   */
    void Add(const std::string& name, std::shared_ptr<KeyConfig> config);

    /** Get a configuration.
  @param name the name of the configuration
  @returns the found configuration
   */
    std::shared_ptr<KeyConfig> Get(const std::string& name) const;

    /** Start configuration auto detection.
   */
    void BeginAutoDetect();

    /** Abort auto detection.
   */
    void AbortAutoDetect();

    /** Get the detected configuration.
  If there are many detected, this will only return the first.
  @returns the first detected configuration
   */
    std::shared_ptr<ConnectedUnits> GetFirstAutoDetectedConfig() const;

    std::map<std::string, std::shared_ptr<KeyConfig>> configs_;
};

namespace config
{
    struct KeyConfigs;
}
void Load(KeyConfigs* configs, const input::config::KeyConfigs& root, const InputActionMap& map);

}  // namespace input
