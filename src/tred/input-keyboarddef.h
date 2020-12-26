/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <memory>

#include "tred/input-key.h"
#include "tred/input-unitdef.h"
#include "tred/input-binddef.h"


namespace input
{
struct InputActionMap;
struct ActiveUnit;
struct InputDirector;

namespace config
{
    struct KeyboardDef;
}

/** Keyboard definition.
 */
struct KeyboardDef : public UnitDef
{
    /** Constructor.
  @param data the data structure to load from
  @param map the input actions to use
   */
    KeyboardDef(const config::KeyboardDef& data, const InputActionMap& map);

    /** Create a active keyboard interface.
  @param director the input director
  @returns the active keyboard
   */
    std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map) override;

    std::vector<BindDef<Key>> keys_;
};

}  // namespace input
