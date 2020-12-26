#include "tred/input-activeaxistorange.h"
#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/input-bind.h"
#include "tred/input-action.h"



namespace input {

ActiveAxisToRange::ActiveAxisToRange(InputAction* action, Bind* axis)
    : InputActive(action), axis_(axis) {
    assert(axis_);

  if (axis_->type() != BindType::AXIS) {
    const std::string error = fmt::format("bound type for axis {} is not a axis, is {}", action->name(), axis_->type());
    throw error;
  }
}

void ActiveAxisToRange::Update(float) {
    assert(axis_);
  float v = axis_->value();
  if (v < 0) {
    v = 0;
  }
  set_state(v);
}

}  // namespace input

