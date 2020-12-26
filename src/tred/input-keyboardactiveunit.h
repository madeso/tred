/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <map>
#include <memory>

#include "tred/input-activeunit.h"
#include "tred/input-trangebind.h"

#include "tred/input-key.h"


namespace input
{
struct AxisKey;
struct InputDirector;
struct BindData;

/** A active keyboard.
 */
struct KeyboardActiveUnit : public ActiveUnit
{
    /** Constructor.
  @param binds the key binds
  @param director the director
   */
    KeyboardActiveUnit(const std::vector<std::shared_ptr<TRangeBind<Key>>>& binds, InputDirector* director);

    /** On key handler.
  @param key the key
  @param state the state of the button
   */
    void OnKey(const Key& key, bool state);

    /** Destructor.
   */
    ~KeyboardActiveUnit();

    /** Rumble function, not really useful.
   */
    void Rumble() override;

    InputDirector* director_;
    const std::map<Key, BindData> actions_;
};

}  // namespace input
