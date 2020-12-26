/** @file
Classes for input handling.
 */

#pragma once
#include <vector>
#include <memory>



namespace input {

struct InputAction;

/** Represents a active unit.
A perfect example is a connected joystick.
 */
struct ActiveUnit {
   /** Destructor.
   */
  virtual ~ActiveUnit();

  /** Causes the active unit to rumble.
   */
  virtual void Rumble() = 0;

 protected:
  /** Constructor.
   */
  ActiveUnit();
};

}  // namespace input




