// Euphoria - Copyright (c) Gustav

/** @file
Classes for input handling.
 */

#ifndef EUPHORIA_INPUT_JOYSTICKACTIVEUNIT_H_
#define EUPHORIA_INPUT_JOYSTICKACTIVEUNIT_H_

#include <vector>
#include <map>

#include "tred/input-axis.h"
#include "tred/input-hataxis.h"
#include "tred/input-activeunit.h"
#include "tred/input-taxisbind.h"
#include "tred/input-trangebind.h"



struct Table;

namespace input {

struct BindData;
struct InputDirector;

/** A active mouse binding
 */
struct JoystickActiveUnit : public ActiveUnit {
 public:
  /** Constructor.
  @param axis the axis binds to use
  @param director the input director
   */
  JoystickActiveUnit(
      int joystick, InputDirector* director,
      const std::vector<std::shared_ptr<TAxisBind<int>>>& axis,
      const std::vector<std::shared_ptr<TRangeBind<int>>>& buttons,
      const std::vector<std::shared_ptr<TAxisBind<HatAxis>>>& hats);

  /** React to a change in the axis.
  @param axis the axis
  @param state the state of the axis
   */
  void OnAxis(int axis, float state);

  void OnButton(int button, float state);

  void OnHat(const HatAxis& hatAxis, float state);

  /** Destructor.
   */
  ~JoystickActiveUnit();

  /** Rumble the joystick.
  Doesn't do anything.
   */
  void Rumble() override;

 private:
  // int joystick_;
  InputDirector* director_;
  std::map<int, BindData> axis_;
  std::map<int, BindData> buttons_;
  std::map<HatAxis, BindData> hats_;
};

}  // namespace input



#endif  // EUPHORIA_INPUT_JOYSTICKACTIVEUNIT_H_
