/** @file
Classes for input handling.
 */

#pragma once
#include <string>

#include "tred/input-range.h"



struct Table;

namespace input {

struct GlobalToggle;

/** A input action.
 */
struct InputAction {
  /** Constructor.
  @param scriptvarname the name of the variable in the script.
  @param range the range of valid values for the scriptvar
  @param global true if this is marked as a global
   */
  InputAction(const std::string& name, const std::string& scriptvarname,
              Range range, bool global);
  ~InputAction();

  /** Get the name of the script var.
   */
  const std::string& scriptvarname() const;

  /** Gets the range of this input.
  @returns the range of this input
   */
  Range range() const;

  const std::string& name() const;

  bool global() const;
  GlobalToggle* toggle();

  void Setup(GlobalToggle* toggle);
  void ClearToggle(GlobalToggle* toggle);

  std::string name_;
  std::string scriptvarname_;
  Range range_;
  bool global_;
  GlobalToggle* toggle_;
};

}  // namespace input




