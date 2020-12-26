/** @file
Classes for input handling.
 */

#pragma once
#include "tred/input-active.h"



namespace input {

struct InputAction;
struct Bind;

/** A input active range.
 */
struct ActiveRangeToAxis : public InputActive {
   /** Constructor.
  @param action the action this active belongs to
   */
  ActiveRangeToAxis(InputAction* action, Bind* positive, Bind* negative);

  void Update(float dt);

   Bind* positive_;
  Bind* negative_;
};

}  // namespace input




