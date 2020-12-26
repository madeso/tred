/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <memory>

#include "tred/input-connectedunits.h"



namespace input {

struct UnitDef;
struct InputDirector;
struct InputActionMap;
struct BindMap;

/** Contains a list of configurations.
A good example is Mouse+Keyboard.
 */
struct KeyConfig {
   /** Constructor.
   */
  KeyConfig();

  /** Add a definition.
  @param def the definition to add.
   */
  void Add(std::shared_ptr<UnitDef> def);

  /** Connect.
  @param director the input director
  @returns the connected units
   */
  std::shared_ptr<ConnectedUnits> Connect(const InputActionMap& actions,
                                          InputDirector* director);

   std::vector<std::shared_ptr<UnitDef>> definitions_;
  std::shared_ptr<BindMap> binds_;
};

}  // namespace input




