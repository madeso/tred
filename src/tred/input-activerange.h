/** @file
Classes for input handling.
 */

#pragma once
#include "tred/input-active.h"


namespace input
{
struct InputAction;
struct Bind;

/** A input active range.
 */
struct ActiveRange : public InputActive
{
    /** Constructor.
  @param action the action this active belongs to
   */
    ActiveRange(InputAction* action, Bind* range);

    void Update(float dt);

    Bind* range_;
};

}  // namespace input
