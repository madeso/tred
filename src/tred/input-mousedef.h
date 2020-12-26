// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_MOUSEDEF_H_
#define EUPHORIA_INPUT_MOUSEDEF_H_

#include <vector>

#include "tred/input-unitdef.h"
#include "tred/input-axis.h"
#include "tred/input-binddef.h"
#include "tred/input-key.h"

namespace config { struct MouseDef; }



namespace input {
class InputActionMap;

/** Mouse definition.
 */
class MouseDef : public UnitDef {
 public:
  /** Constructor.
  @param data the data structure to load from
  @param map the input actions to use
   */
  MouseDef(const config::MouseDef& data, const InputActionMap& map);

  /** Create a active mouse interface.
  @param director the input director
  @returns the active mouse
   */
  std::shared_ptr<ActiveUnit> Create(InputDirector* director,
                                     BindMap* map) override;

 private:
  std::vector<BindDef<Axis>> axis_;
  std::vector<BindDef<MouseButton>> keys_;
};
}  // namespace input



#endif  // EUPHORIA_INPUT_MOUSEDEF_H_
