#include "tred/input-action.h"
#include <cassert>



namespace input {

InputAction::InputAction(const std::string& name,
                         const std::string& scriptvarname, Range range,
                         bool global)
    : name_(name),
      scriptvarname_(scriptvarname),
      range_(range),
      global_(global),
      toggle_(0) {
  }

InputAction::~InputAction() {
    assert(toggle_ == 0);
}

const std::string& InputAction::scriptvarname() const {
    return scriptvarname_;
}

Range InputAction::range() const {
    return range_;
}

const std::string& InputAction::name() const {
    return name_;
}

bool InputAction::global() const {
    return global_;
}

GlobalToggle* InputAction::toggle() {
    return toggle_;
}

void InputAction::Setup(GlobalToggle* toggle) {
    assert(toggle);
  assert(toggle_ == 0);
  toggle_ = toggle;
}

void InputAction::ClearToggle(GlobalToggle* toggle) {
    assert(toggle);
  assert(toggle == toggle_);
  toggle_ = 0;
}

}  // namespace input

