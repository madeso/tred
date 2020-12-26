// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_JOYSTICKDEF_H_
#define EUPHORIA_INPUT_JOYSTICKDEF_H_

#include <vector>

#include "tred/input-unitdef.h"
#include "tred/input-binddef.h"
#include "tred/input-hataxis.h"

namespace config { struct JoystickDef; }



namespace input {

struct InputDirector;
struct ActiveUnit;
struct InputActionMap;

/** Joystick definition.
 */
struct JoystickDef : public UnitDef {
 public:
  /** Constructor.
  @param data the data structure to load from
   */
  JoystickDef(const config::JoystickDef& data, const InputActionMap& map);

  /** Create a active joystick interface.
  @param director the input director
  @returns the active joystick
   */
  std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map);

 private:
  std::vector<BindDef<int>> axis_;
  std::vector<BindDef<int>> buttons_;
  std::vector<BindDef<HatAxis>> hats_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_JOYSTICKDEF_H_
